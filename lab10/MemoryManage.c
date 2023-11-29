#include "MemoryManage.h"

void AllocNodeInit(AllocatedSpace *node, int startaddr, int len, char j)
{
	assert(node != NULL);
	node->start_address = startaddr;
	node->length = len;
	node->job = j;
	node->next = NULL;
}

void FreeNodeInit(FreeSpace *node, int startaddr, int len)
{
	assert(node != NULL);
	node->start_address = startaddr;
	node->length = len;
	node->next = NULL;
}

// 注意我们要对一级指针进行修改，需要传递二级指针
void InitSpacePtr(AllocatedSpace **allocate_head_ptr, FreeSpace **free_head_ptr)
{
	assert(allocate_head_ptr != NULL);
	assert(free_head_ptr != NULL);

	if (*allocate_head_ptr != NULL || *free_head_ptr != NULL)
		logMessage(WARNING, "初始化时分配头和空闲头指针非空，可能引发内存泄漏! alloc: %p free: %p",
				   *allocate_head_ptr, *free_head_ptr);

	// 使用哨兵位节点作为头，简化后面对单链表的增删操作
	*allocate_head_ptr = (AllocatedSpace *)malloc(sizeof(AllocatedSpace));
	AllocatedSpace *alloc_dumphead = *allocate_head_ptr;
	if (alloc_dumphead == NULL)
	{
		logMessage(FATAL, "malloc failed");
		exit(-1);
	}
	// alloc_dumphead->start_address = -1;
	// alloc_dumphead->length = -1;
	// alloc_dumphead->job = '\0';
	// alloc_dumphead->next = NULL;
	AllocNodeInit(alloc_dumphead, -1, -1, (char)'\0');

	*free_head_ptr = (FreeSpace *)malloc(sizeof(FreeSpace));
	FreeSpace *free_dumphead = *free_head_ptr;
	if (free_dumphead == NULL)
	{
		logMessage(FATAL, "malloc failed");
		exit(-1);
	}
	// free_dumphead->start_address = -1;
	// free_dumphead->length = -1;
	FreeNodeInit(free_dumphead, -1, -1);
	free_dumphead->next = (FreeSpace *)malloc(sizeof(FreeSpace));

	FreeSpace *real_free_head = free_dumphead->next;
	if (real_free_head == NULL)
	{
		logMessage(FATAL, "malloc failed");
		exit(-1);
	}
	// 初始化真正的头节点
	// real_free_head->start_address = START_ADDR;
	// real_free_head->length = MEM_SIZE;
	// real_free_head->next = NULL;
	FreeNodeInit(real_free_head, START_ADDR, MEM_SIZE);
}

// 分配空间的链表比较简单，头插就行了
// 注意此时插入的head是分配空间表的哨兵位头节点
// 为了配合Show方法，此处改为按开始地址从低高顺序插入
void AllocInsert(AllocatedSpace *head, AllocatedSpace *node)
{
	assert(head != NULL);
	assert(node != NULL);

	// AllocatedSpace *next = head->next;
	// head->next = node;
	// node->next = next;
	// 找第一个开始地址大于node的！由于是单链表的插入，需要用一个prev指针记录前一个节点
	AllocatedSpace *cur = head->next;
	AllocatedSpace *prev = head;
	while (cur != NULL)
	{
		if (cur->start_address < node->start_address)
		{
			prev = cur;
			cur = cur->next;
		}
		else
			break;
	}
	// prev node cur
	prev->next = node;
	node->next = cur;
}

// 展示内存分区情况，传入参数是哨兵位节点
void Show(AllocatedSpace *allocate_dumphead_ptr, FreeSpace *free_dumphead_ptr, int option)
{
	assert(allocate_dumphead_ptr != NULL && free_dumphead_ptr != NULL);
	// 对于bestfit和worstfit，未分配区按照length排序，为了按照addr顺序展示分区，打印前按照addr排，打印后再按照length排回来
	if (option == 2 || option == 3)
		free_dumphead_ptr->next = listMergeSort(free_dumphead_ptr->next, nodeCmpByAddr);
	AllocatedSpace *alloc_head = allocate_dumphead_ptr->next;
	FreeSpace *free_head = free_dumphead_ptr->next;
	int i = 0;

	printf("**************************************************\n");
	printf("*                  内存分区情况                   *\n");
	printf("**************************************************\n");
	printf(" 分区\t\t起始地址\t结束地址\t大小\t占用情况\n");
	while (alloc_head != NULL && free_head != NULL)
	{
		if (alloc_head->start_address < free_head->start_address)
		{
			printf("分区%d\t\t%6d\t\t%6d\t%6dB\t\t占用(%c)\n",
				   i, alloc_head->start_address,
				   alloc_head->start_address + alloc_head->length,
				   alloc_head->length, alloc_head->job);
			alloc_head = alloc_head->next;
		}
		else
		{
			printf("分区%d\t\t%6d\t\t%6d\t%6dB\t\t空闲\n",
				   i, free_head->start_address,
				   free_head->start_address + free_head->length,
				   free_head->length);
			free_head = free_head->next;
		}
		i++;
	}
	while (alloc_head != NULL)
	{
		printf("分区%d\t\t%6d\t\t%6d\t%6dB\t\t占用(%c)\n",
			   i, alloc_head->start_address,
			   alloc_head->start_address + alloc_head->length,
			   alloc_head->length, alloc_head->job);
		alloc_head = alloc_head->next;
	}
	while (free_head != NULL)
	{
		printf("分区%d\t\t%6d\t\t%6d\t%6dB\t\t空闲\n",
			   i, free_head->start_address,
			   free_head->start_address + free_head->length,
			   free_head->length);
		free_head = free_head->next;
	}
	printf("**************************************************\n");
	// bestfit要用best的比较器
	if (option == 2)
		free_dumphead_ptr->next = listMergeSort(free_dumphead_ptr->next, nodeCmpByLen_Best);
	// worstfit要用worst比较器
	if (option == 3)
		free_dumphead_ptr->next = listMergeSort(free_dumphead_ptr->next, nodeCmpByLen_Worst);
}

int alloc_check(AllocatedSpace *head1, FreeSpace *head2, char job, int size)
{
	if (size <= 0)
	{
		logMessage(WARNING, "%c要求分配内存大小非法(<=0)", job);
		return -1;
	}
	assert(head2 != NULL && head2->next != NULL);
	assert(head1 != NULL);
	return 0;
}

// 向FreeList中归还tar_node所占有的资源，传入的head为FreeList的虚拟头节点
int turnBackMemory(AllocatedSpace *tar_node, FreeSpace *head)
{
	int end = tar_node->start_address + tar_node->length;
	// 在freeList中找第一个start_address大于等于end的节点
	// 由于可能存在向前合并的情况，每次需要记录前一个节点
	FreeSpace *prev = head;
	FreeSpace *cur = head->next;
	while (cur != NULL)
	{
		if (cur->start_address < end)
		{
			prev = cur;
			cur = cur->next;
		}
		else
		{
			int prev_end = prev->start_address + prev->length;
			// 需要向后合并
			if (cur->start_address == end)
			{
				// 在哨兵位节点后面归还空间 + 向后合并
				if (prev == head)
				{
					// 不需要插入新节点， 直接对cur这块的开始地址和长度进行修改即可
					cur->start_address -= tar_node->length;
					cur->length += tar_node->length;
				}
				// 向前向后合并
				else if (prev_end == tar_node->start_address)
				{
					// prev cur
					// prev tar_node cur
					// 把cur这块删掉，更新prev的长度即可
					prev->next = cur->next;
					prev->length = prev->length + tar_node->length + cur->length;
					free(cur);
					cur = NULL;
				}
				// 向后合并
				else if (prev_end < tar_node->start_address)
				{
					// 只需要修改cur的开始地址和长度，其实和第一个情况一样
					// 两种情况可以合并，但是为了可读性，将这个情况完整写出
					cur->start_address -= tar_node->length;
					cur->length += tar_node->length;
				}
				// 不应该存在该情况，打印一条日志
				else
				{
					logMessage(WARNING, "尝试归还%c的资源失败，尝试归还的资源空间与前面空闲资源空间产生交叉", tar_node->job);
					return -1;
				}
			}
			// end < 后面第一个空闲资源的起始地址
			// 不需要向后合并，只需考虑向前合并
			else
			{
				// 在哨兵位节点后面归还空间
				if (prev == head)
				{
					// 需要插入新节点
					FreeSpace *newnode = (FreeSpace *)malloc(sizeof(FreeSpace));
					FreeNodeInit(newnode, tar_node->start_address, tar_node->length);
					prev->next = newnode;
					newnode->next = cur;
				}
				// 向前合并
				else if (prev_end == tar_node->start_address)
				{
					// 只需要修改prev节点的len即可
					prev->length += tar_node->length;
				}
				// 无需合并，直接插入新节点即可
				else if (prev_end < tar_node->start_address)
				{
					// prev后面插入新节点即可，其实和第一个情况一样
					// 两种情况可以合并，但是为了可读性，将这个情况完整写出
					FreeSpace *newnode = (FreeSpace *)malloc(sizeof(FreeSpace));
					FreeNodeInit(newnode, tar_node->start_address, tar_node->length);
					prev->next = newnode;
					newnode->next = cur;
				}
				// 不存在该情况，打印日志即可
				else
				{
					logMessage(WARNING, "尝试归还%c的资源失败，尝试归还的资源空间与前面空闲资源空间产生交叉", tar_node->job);
					return -1;
				}
			}
			return 0;
		}
	}
	if (cur == NULL)
	{
		logMessage(WARNING, "归还%c失败", tar_node->job);
		return -1;
	}
}

int nodeCmpByAddr(FreeSpace *n1, FreeSpace *n2)
{
	return n1->start_address < n2->start_address;
}

int nodeCmpByLen_Best(FreeSpace *n1, FreeSpace *n2)
{
	return n1->length < n2->length;
}

int nodeCmpByLen_Worst(FreeSpace *n1, FreeSpace *n2)
{
	return n1->length > n2->length;
}

// 归并排序
FreeSpace *findMid(FreeSpace *list)
{
	// node1 node2 node3 --> mid = node2
	// node1 node2 node3 node4 --> mid = node2
	FreeSpace *fast = list;
	FreeSpace *slow = list;
	while (fast != NULL && fast->next != NULL && fast->next->next != NULL)
	{
		fast = fast->next->next;
		slow = slow->next;
	}
	return slow;
}

FreeSpace *listMerge(FreeSpace *left, FreeSpace *right, int (*cmp)(FreeSpace *, FreeSpace *))
{
	FreeSpace *dummyFreeSpace = (FreeSpace *)malloc(sizeof(FreeSpace));
	FreeNodeInit(dummyFreeSpace, -1, -1);
	FreeSpace *head = dummyFreeSpace;
	while (left != NULL && right != NULL)
	{
		if (cmp(left, right))
		{
			head->next = left;
			left = left->next;
		}
		else
		{
			head->next = right;
			right = right->next;
		}
		head = head->next;
	}
	if (left != NULL)
		head->next = left;
	if (right != NULL)
		head->next = right;
	FreeSpace *ret = dummyFreeSpace->next;
	free(dummyFreeSpace);
	return ret;
}

// 排序后得到不带哨兵位的链表
FreeSpace *listMergeSort(FreeSpace *list, int (*cmp)(FreeSpace *, FreeSpace *))
{
	if (list == NULL || list->next == NULL)
		return list;
	// 利用快慢指针找到中间节点
	FreeSpace *mid = findMid(list);
	// 从中间节点节点断开链表
	FreeSpace *mid_next = mid->next;
	mid->next = NULL;
	// 归并排序左右两段链表
	FreeSpace *left = listMergeSort(list, cmp);
	FreeSpace *right = listMergeSort(mid_next, cmp);
	// 将排好序的左右链表归并
	return listMerge(left, right, cmp);
}

// 由于worstfit可以复用bestfit的free函数
// 将原来实现的bestfit的free函数抽象出主体，根据传入比较器的不同来控制是bestfit还是worstfit
// 传入的两个表头节点都是虚拟头节点即哨兵位节点
int _basic_free(char job, int (*cmp)(FreeSpace *, FreeSpace *), AllocatedSpace* allocated_header, FreeSpace* free_header)
{
    assert(allocated_header != NULL && free_header != NULL && free_header->next != NULL);
    // 在分配表中查找需要归还空间的node
    AllocatedSpace *cur = allocated_header->next;
    AllocatedSpace *prev = allocated_header;
    while (cur != NULL)
    {
        if (cur->job == job)
            break;
        else
        {
            prev = cur;
            cur = cur->next;
        }
    }
    // prev为待删除节点的前一个节点，cur为待删除节点
    // 不存在该节点
    if(cur == NULL)
    {
        logMessage(DEBUG, "尝试释放%c的空间失败, 当前系统并未给%c分配空间", job, job);
        return -1;
    }
    // 存在该任务，释放该任务的空间
    // 1. 分配空间链表中删除该节点
    // 2. 向未分配空间链表中释放空间
    // 3. 要求分配链表按照节点空间大小排序
    // 4. 插入新节点或者合并时都需要按照大小排序
    else
    {
        // 在分配表中删除该任务节点
        prev->next = cur->next;
        // 向未分配表中归还任务节点所占据的资源
        // 考虑到合并实现起来确实复杂
        // 偷个懒，把按照size排序的未分配表按照addr大小归并排序后直接复用FirstFit的turnBackMemory，之后再按照size排一次
        // 人生苦短，我会偷懒
        free_header->next = listMergeSort(free_header->next, nodeCmpByAddr);
        int ret = turnBackMemory(cur, free_header);
        free_header->next = listMergeSort(free_header->next, cmp);
        // 释放该节点在堆上开辟的空间
        free(cur);
        return ret;
    }
}