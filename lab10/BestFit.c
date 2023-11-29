#include "BestFit.h"
#include "FirstFit.h"

int BestFit_alloc(char job, int size)
{
    // 复用FirstFit的分配函数，逻辑一样，只需要在归还空间的时候按照size进行插入
    return FirstFit_alloc(job, size);
}

int BestFit_free(char job)
{
    return _basic_free(job, nodeCmpByLen_Best,
                    allocated_header, free_header);
}