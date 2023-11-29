#pragma once
#include "MemoryManage.h"

extern AllocatedSpace *allocated_header;
extern FreeSpace *free_header;

int FirstFit_alloc(char, int);
int FirstFit_free(char);

// 将该函数放到MemoryManage.c中，让不同算法复用
// int turnBackMemory(AllocatedSpace*);