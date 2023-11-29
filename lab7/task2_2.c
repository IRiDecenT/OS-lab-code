#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <semaphore.h>
#include <pthread.h>

// 利用线程返回值获取单词统计数目

struct  buf
{
    char *filename;
    int wc_count;
}arg[2];


void* count_words(void* arg)
{
    struct buf *b = (struct buf*)arg;
    printf("%s words counting begin\n", b->filename);
    FILE* fp;
    int c, prevc = '\0';
    if((fp = fopen(b->filename, "r")) != NULL)
    {
        while((c = getc(fp)) != EOF)
        {
            if(!isalnum(c) && isalnum(prevc))
            {
                b->wc_count++;
            }
            prevc = c;
        }
        fclose(fp);
    }
    else
        perror(b->filename);
    printf("%s words counting done\n", b->filename);
    return NULL;
}


int main(int argc, char* argv[])
{
    if(3 != argc)
    {
        printf("Usage:\n\t%s file1 file2\n", argv[0]);
        exit(-1);
    }


    pthread_t fileReaderThread1, fileReaderThread2;

    arg[0].filename = argv[1];
    arg[0].wc_count = 0;
    arg[1].filename = argv[2];
    arg[1].wc_count = 0;

    pthread_create(&fileReaderThread1, NULL, count_words, (void*)&arg[0]);
    pthread_create(&fileReaderThread2, NULL, count_words, (void*)&arg[1]);

    pthread_join(fileReaderThread1, NULL);
    pthread_join(fileReaderThread2, NULL);

    printf("%s与%s文件中的单词总数为: %d\n", arg[0].filename, arg[1].filename, arg[0].wc_count + arg[1].wc_count);
    return 0;
}
