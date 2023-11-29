#pragma once

#include "MemoryManage.h"
#include "FirstFit.h"
#include <time.h>
#include <stdlib.h>

extern AllocatedSpace *allocated_header;
extern FreeSpace *free_header;

char jobs[8] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H'};

void PrintAllocList(AllocatedSpace *head)
{
    assert(head != NULL);
    printf("AllocList:");
    while(head != NULL)
    {
        printf("(%d, %d, %c, %p)->",
            head->start_address,
            head->length,
            head->job,
            head->next);
        head = head->next;
    }
    printf("null\n");
}

void PrintFreeList(FreeSpace *head)
{
    assert(head != NULL);
    printf("FreeList:");
    while(head != NULL)
    {
        printf("(%d, %d, %p)->",
            head->start_address,
            head->length,
            head->next);
        head = head->next;
    }
    printf("null\n");
}

void PrintAll()
{
    PrintAllocList(allocated_header);
    PrintFreeList(free_header);
}

void InitTest()
{
    assert(allocated_header != NULL);
    assert(free_header != NULL);
    PrintAll();
    printf("\n");
}

void FirstfitAllocTest()
{
    FirstFit_alloc('A', 100);
    FirstFit_alloc('B', 200);
    FirstFit_alloc('C', 300);
    FirstFit_alloc('D', 400);
    FirstFit_alloc('E', 100);
    FirstFit_alloc('F', 200);
    FirstFit_alloc('G', 300);
    FirstFit_alloc('H', 400);
    PrintAll();
}

void FirstFitFreeTest()
{
    FirstfitAllocTest();
    for(int i = 0; i < 10; i++)
    {
        char c = jobs[rand()%8];
        printf("即将归还%c\n", c);
        FirstFit_free(c);
        PrintAll();
    }
}
