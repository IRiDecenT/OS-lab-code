#pragma once
#include "MemoryManage.h"

extern AllocatedSpace *allocated_header;
extern FreeSpace *free_header;

int BestFit_alloc(char, int);
int BestFit_free(char);

// 改为使用公用的接口
// int turnBackMemory_best(AllocatedSpace*);