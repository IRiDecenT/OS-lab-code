#include <string.h>

#include "MemoryManage.h"
#include "Test.h"
#include "FirstFit.h"
#include "NextFit.h"
#include "BestFit.h"
#include "WorstFit.h"
#include "Log.h"
AllocatedSpace *allocated_header = NULL;
FreeSpace *free_header = NULL;

int algo_choice = 0;

int (*alloc_handler[4])(char, int) = {
	FirstFit_alloc,
	NextFit_alloc,
	BestFit_alloc,
	WorstFit_alloc
};

int (*free_handler[4])(char) = {
	FirstFit_free,
	NextFit_free,
	BestFit_free,
	WorstFit_free
};

char* algoName[] = {
	"FirstFit",
	"NextFit",
	"BestFit",
	"WorstFit"
};



void Display()
{
	printf("__________________________________________________________________________________________\n");
	printf("|                                                                                         |\n");
	printf("|                                连续空间存储管理模拟实验                                   |\n");
	printf("|                                    %s 算法                                        |\n", algoName[algo_choice]);
	printf("|                                       0.退出                                            |\n");
	printf("|                                  1.为作业分配内存                                        |\n");
	printf("|                                     2.回收内存                                          |\n");
	printf("|                                    3.显示分配情况                                        |\n");
	printf("|_________________________________________________________________________________________|\n");
	printf("请输入选择:>");
}

void Alloc()
{
	printf("分配内存，输入格式:[作业名] [空间大小]\n");
	printf("请输入:>");
	char job;
	int size;
	getchar();
	scanf("%c %d", &job, &size);
	printf("%c %d\n", job, size);
	if (alloc_handler[algo_choice](job, size) != 0)
	{
		printf("分配失败！\n");
	}
	else
	{
		printf("成功为%c分配%d字节\n", job, size);
	}
}

void Free()
{
	printf("回收内存，输入格式:[作业名]\n");
	printf("请输入:>");
	char job;
	getchar();
	scanf("%c", &job);
	if (free_handler[algo_choice](job) != 0)
	{
		printf("回收失败！\n");
	}
	else
	{
		printf("成功回收%c的内存空间\n", job);
	}
}

void ShowMemory()
{
	PrintAll();
	Show(allocated_header, free_header, algo_choice);
}

void Run()
{
	int input = 0;
	do
	{
		Display();
		scanf("%d", &input);
		switch (input)
		{
		case 0:
			break;
		case 1:
			Alloc();
			break;
		case 2:
			Free();
			break;
		case 3:
			ShowMemory();
			break;
		default:
			printf("输入错误选项，请重新选择！\n");
			break;
		}
	} while (input != 0);
	printf("Bye!\n");
}

void OutputRedir()
{
	umask(0);
	int fd = open(LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0666);
	assert(fd > 0);
	dup2(fd, 1);
	dup2(fd, 2);
}

int main(int argc, char *argv[])
{
	if(argc != 1)
	{
		algo_choice = atoi(argv[1]);
	}
	InitSpacePtr(&allocated_header, &free_header);
	prevNodeInit(); // NextFit 需要初始化前一次扫描的节点
	// InitTest(allocated_header, free_header);
	// FirstfitAllocTest();
	// FirstFitFreeTest();
	for(int i = 0; i < argc; i++)
	{
		if(strcmp(argv[i], "-dup") == 0)
			OutputRedir();
	}
	Run();
	return 0;
}