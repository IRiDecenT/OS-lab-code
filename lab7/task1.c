#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

static volatile int counter = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
void* mythread(void* arg)
{
    printf("%s: begin\n", (char*) arg);
    for(int i = 0; i < 1e7; i++)
    {
        pthread_mutex_lock(&mutex);
        counter += 1;
        pthread_mutex_unlock(&mutex);
    }
    printf("%s: done\n", (char*)arg);
    return NULL;
}
int main()
{
    pthread_t tid1, tid2;
    printf("main: begin (counter = %d)\n", counter);
    pthread_create(&tid1, NULL, mythread, "thread A");
    pthread_create(&tid2, NULL, mythread, "thread B");
    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);
    printf("main: done with both (counter = %d)\n", counter);
    return 0;
}
