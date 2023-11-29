#include "NextFit.h"
#include "FirstFit.h"

// 上次扫描的结束处
FreeSpace* prevNode;
// 标志在分配时是否走了一个轮次，从prevNode走到尾部即说明从prevNode开始往后扫描了
// 结合flag就可以判断head是否从prevNode走了一圈再走到prevNode
int flag = 0;

void prevNodeInit()
{
    prevNode = free_header->next;
}

int NextFit_alloc(char job, int size)
{
    FreeSpace *head = prevNode;
    while (1)
    {
        if (head->length < size)
        {
            head = head->next;
        }
        else
        {
            AllocatedSpace *node = (AllocatedSpace *)malloc(sizeof(AllocatedSpace));
            AllocNodeInit(node, head->start_address, size, job);
            AllocInsert(allocated_header, node);
            head->start_address = head->start_address + size;
            head->length -= size;
            // 更新结束扫描处
            prevNode = head;
            return 0;
        }
        if(head == NULL)
        {
            head = free_header->next;
            flag = 1;
        }
        // 从头走到尾走了一圈走到前一次扫描处了，说明没有空间能够返回了
        if(head == prevNode && flag == 1)
        {
            flag = 0;
            logMessage(DEBUG, "内存空间不足以为%c分配%d字节", job, size);
            break;
        }
    }
    return -1;
}
// 释放时直接复用FirstFit即可
int NextFit_free(char job)
{
    return FirstFit_free(job);
}

