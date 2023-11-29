#include "Log.h"

const char* log_level[] = {"DEBUG","NOTICE", "WARNING", "FATAL"};

void logMessage(int level,const char *format, ...)
{
    assert(level >= DEBUG);
    assert(level <= FATAL);

    char* name = getenv("USER");

    char logInfo[1024];
    va_list ap;
    va_start(ap,format);

    vsnprintf(logInfo,sizeof(logInfo)-1,format,ap);

    va_end(ap);

    // umask(0);
    // int fd = open(LOGFILE, O_WRONLY | O_CREAT | O_APPEND, 0666 );
    // assert(fd > 0);
    FILE* out = (level == FATAL)?stderr:stdout;

    // dup2(fd, 1);
    // dup2(fd, 2);

    fprintf(out,"%s | %u | %s | %s \n",\
            log_level[level],\
            (unsigned int)time(NULL),\
            name == NULL?"unknow":name,\
            logInfo);

    fflush(out); //将c缓冲区的数据刷新到操作系统
    // fsync(fd);   //将OS的数据刷到硬盘
}
