#pragma once
#include <stdio.h>
#include <assert.h>
#include "Log.h"
// size of memory   100000bytes
#define MEM_SIZE 100000
// 内存的起始地址，设为0，方便计算
#define START_ADDR 0

typedef struct AllocatedSpace
{
	int start_address;
	int length;
	char job;
	struct AllocatedSpace *next;
} AllocatedSpace;

typedef struct FreeSpace
{
	int start_address;
	int length;
	struct FreeSpace *next;
} FreeSpace;

void AllocNodeInit(AllocatedSpace *node, int startaddr, int len, char j);

void FreeNodeInit(FreeSpace *node, int startaddr, int len);

// 注意我们要对一级指针进行修改，需要传递二级指针
void InitSpacePtr(AllocatedSpace **allocate_head_ptr, FreeSpace **free_head_ptr);

// 分配空间的链表比较简单，头插就行了 ———— abandoned
// 注意此时插入的head是分配空间表的哨兵位头节点
// 为了配合Show方法，此处改为按开始地址从低高顺序插入
void AllocInsert(AllocatedSpace *head, AllocatedSpace *node);

void Show(AllocatedSpace *, FreeSpace *, int);

// 对于分配算法，分配前的合法性检查，传入虚拟头节点（即哨兵位节点）
int alloc_check(AllocatedSpace *, FreeSpace *, char, int);

// 向FreeList中归还tar_node所占有的资源，传入的head为FreeList的虚拟头节点
int turnBackMemory(AllocatedSpace *tar_node, FreeSpace *head);

// 归并排序用于比较的回调函数
int nodeCmpByAddr(FreeSpace *n1, FreeSpace *n2);
int nodeCmpByLen_Best(FreeSpace *n1, FreeSpace *n2);
int nodeCmpByLen_Worst(FreeSpace *n1, FreeSpace *n2);

// 寻找中间节点
FreeSpace *findMid(FreeSpace *list);
// 链表归并
FreeSpace *listMerge(FreeSpace *, FreeSpace *, int (*)(FreeSpace *, FreeSpace *));
// 排序后得到的是不带哨兵位的链表
FreeSpace *listMergeSort(FreeSpace *, int (*)(FreeSpace *, FreeSpace *));

int _basic_free(char, int(*)(FreeSpace*, FreeSpace*), AllocatedSpace*, FreeSpace*);
