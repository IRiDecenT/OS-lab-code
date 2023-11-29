#include "FirstFit.h"

// 要求freeList 每个节点的地址从小到大排序好链接在一起

// 最先适配算法分配空间，失败返回0, 成功返回1
int FirstFit_alloc(char job, int size)
{
    // 检查分配合法性
    if(alloc_check(allocated_header, free_header, job, size) !=0 )
        return -1;
    FreeSpace *head = free_header->next;
    while (head != NULL)
    {
        if (head->length < size)
            head = head->next;
        else
        {
            AllocatedSpace *node = (AllocatedSpace *)malloc(sizeof(AllocatedSpace));
            AllocNodeInit(node, head->start_address, size, job);
            AllocInsert(allocated_header, node);
            head->start_address = head->start_address + size;
            head->length -= size;
            break;
        }
    }
    if (head == NULL)
    {
        logMessage(DEBUG, "内存空间不足以为%c分配%d字节", job, size);
        return -1;
    }
    else
        return 0;
}

int FirstFit_free(char job)
{
    assert(free_header != NULL && free_header->next != NULL);
    assert(allocated_header != NULL);

    // 哨兵位节点的后一个节点才是真正的头节点
    // tar用于找需要释放空间的任务,由于是单链表，要删除节点需要拿到前一个节点
    // 从头开始遍历链表，因为我们设置了哨兵位节点，prev初始就是哨兵位节点，不需要再判空以及删除头节点时特殊处理
    AllocatedSpace *alloc_tar = allocated_header->next;
    AllocatedSpace *alloc_tar_prev = allocated_header;
    while (alloc_tar != NULL)
    {
        // 由于我们设置job标志的是char，如果是char*还得strcmp
        if (alloc_tar->job == job)
            break;
        else
        {
            alloc_tar_prev = alloc_tar;
            alloc_tar = alloc_tar->next;
        }
    }
    // 不存在该节点！当前job没有被分配空间
    if (alloc_tar == NULL)
    {
        logMessage(DEBUG, "尝试释放%c的空间失败, 当前系统并未给%c分配空间", job, job);
        return -1;
    }
    // 存在该任务，释放该任务的空间
    // 1. 从分配链表中将该任务节点删除
    // 2. 向空余空间链表中释放空间， 需要考虑空间合并的问题
    else
    {
        // 在allocList删除任务节点
        alloc_tar_prev->next = alloc_tar->next;
        // 向freeList归还资源, 不负责free(alloc节点) 需要自己手动free
        int ret = turnBackMemory(alloc_tar, free_header);
        free(alloc_tar);
        return ret;
    }
}

// int turnBackMemory(AllocatedSpace *tar_node)
// {
//     int end = tar_node->start_address + tar_node->length;
//     // 在freeList中找第一个start_address大于等于end的节点
//     // 由于可能存在向前合并的情况，每次需要记录前一个节点
//     FreeSpace *prev = free_header;
//     FreeSpace *cur = free_header->next;
//     while (cur != NULL)
//     {
//         if (cur->start_address < end)
//         {
//             prev = cur;
//             cur = cur->next;
//         }
//         else
//         {
//             int prev_end = prev->start_address + prev->length;
//             // 需要向后合并
//             if (cur->start_address == end)
//             {
//                 // 在哨兵位节点后面归还空间 + 向后合并
//                 if (prev == free_header)
//                 {
//                     // 不需要插入新节点， 直接对cur这块的开始地址和长度进行修改即可
//                     cur->start_address -= tar_node->length;
//                     cur->length += tar_node->length;
//                 }
//                 // 向前向后合并
//                 else if (prev_end == tar_node->start_address)
//                 {
//                     // prev cur
//                     // prev tar_node cur
//                     // 把cur这块删掉，更新prev的长度即可
//                     prev->next = cur->next;
//                     prev->length = prev->length + tar_node->length + cur->length;
//                     free(cur);
//                     cur = NULL;
//                 }
//                 // 向后合并
//                 else if (prev_end < tar_node->start_address)
//                 {
//                     // 只需要修改cur的开始地址和长度，其实和第一个情况一样
//                     // 两种情况可以合并，但是为了可读性，将这个情况完整写出
//                     cur->start_address -= tar_node->length;
//                     cur->length += tar_node->length;
//                 }
//                 // 不应该存在该情况，打印一条日志
//                 else
//                 {
//                     logMessage(WARNING, "尝试归还%c的资源失败，尝试归还的资源空间与前面空闲资源空间产生交叉", tar_node->job);
//                     return -1;
//                 }
//             }
//             // end < 后面第一个空闲资源的起始地址
//             // 不需要向后合并，只需考虑向前合并
//             else
//             {
//                 // 在哨兵位节点后面归还空间
//                 if (prev == free_header)
//                 {
//                     // 需要插入新节点
//                     FreeSpace *newnode = (FreeSpace*)malloc(sizeof(FreeSpace));
//                     FreeNodeInit(newnode, tar_node->start_address, tar_node->length);
//                     prev->next = newnode;
//                     newnode->next = cur;
//                 }
//                 // 向前合并
//                 else if (prev_end == tar_node->start_address)
//                 {
//                     // 只需要修改prev节点的len即可
//                     prev->length += tar_node->length;
//                 }
//                 // 无需合并，直接插入新节点即可
//                 else if (prev_end < tar_node->start_address)
//                 {
//                     // prev后面插入新节点即可，其实和第一个情况一样
//                     // 两种情况可以合并，但是为了可读性，将这个情况完整写出
//                     FreeSpace *newnode = (FreeSpace*)malloc(sizeof(FreeSpace));
//                     FreeNodeInit(newnode, tar_node->start_address, tar_node->length);
//                     prev->next = newnode;
//                     newnode->next = cur;
//                 }
//                 // 不存在该情况，打印日志即可
//                 else
//                 {
//                     logMessage(WARNING, "尝试归还%c的资源失败，尝试归还的资源空间与前面空闲资源空间产生交叉", tar_node->job);
//                     return -1;
//                 }
//             }
//             return 0;
//         }
//     }
//     if(cur==NULL)
//     {
//         logMessage(WARNING, "归还%c失败", tar_node->job);
//         return -1;
//     }
// }
