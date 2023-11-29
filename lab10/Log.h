#pragma once
#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define DEBUG 0
#define NOTICE 1
#define WARNING 2
#define FATAL 3
#define LOGFILE "MemoryManage.log"

void logMessage(int level,const char *format, ...);
