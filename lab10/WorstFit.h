#include "MemoryManage.h"

extern AllocatedSpace *allocated_header;
extern FreeSpace *free_header;

// 实现了BestFit后后续都可以直接复用
// 只需要在free时将排序方式改一下，根据我们对归并排序的设计，只需要新增一个函数指针即可

int WorstFit_alloc(char, int);
int WorstFit_free(char);