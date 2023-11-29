#pragma once
#include "MemoryManage.h"

extern AllocatedSpace *allocated_header;
extern FreeSpace *free_header;


void prevNodeInit();
int NextFit_alloc(char, int);
int NextFit_free(char);