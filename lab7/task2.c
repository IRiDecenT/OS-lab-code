#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <semaphore.h>
#include <pthread.h>
sem_t mutex;
int total_words = 0;
void* count_words(void* arg)
{
    char* fileName = (char*) arg;
    printf("%s words counting begin\n", fileName);
    FILE* fp;
    int c, prevc = '\0';
    if((fp = fopen(fileName, "r")) != NULL)
    {
        while((c = getc(fp)) != EOF)
        {
            if(!isalnum(c) && isalnum(prevc))
            {
                sem_wait(&mutex);
                total_words++;
                sem_post(&mutex);
            }
            prevc = c;
        }
        fclose(fp);
    }
    else
        perror(fileName);
    printf("%s words counting done\n", fileName);
    return NULL;
}


int main(int argc, char* argv[])
{
    if(3 != argc)
    {
        printf("Usage:\n\t%s file1 file2\n", argv[0]);
        exit(-1);
    }


    sem_init(&mutex, 0, 1); // 相当于互斥锁
    pthread_t fileReaderThread1, fileReaderThread2;
    pthread_create(&fileReaderThread1, NULL, count_words, argv[1]);
    pthread_create(&fileReaderThread2, NULL, count_words, argv[2]);

    pthread_join(fileReaderThread1, NULL);
    pthread_join(fileReaderThread2, NULL);

    printf("%s与%s文件中的单词总数为: %d\n", argv[1], argv[2], total_words);
    sem_destroy(&mutex);
    return 0;
}
