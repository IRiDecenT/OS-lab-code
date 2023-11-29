#include <iostream>
#include <cstdio>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>
#include <cstdlib>

// 场景：当前有一个可以放置两个水果的盘子
// father往盘子中放苹果，mother往盘子中放橘子
// son只从盘子中拿橘子吃，daugter只从盘子中拿苹果吃
// 完成四者的互斥与同步

sem_t orange;
sem_t apple;
sem_t plate;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int appleNum = 0;
int orangeNum = 0;


std::string plateInfo()
{
    std::string ret = std::string("盘子中🍎: ") + std::to_string(appleNum)
                + std::string(" 🍊: ") + std::to_string(orangeNum);
    return ret;
}

void *placeApple(void *arg)
{
    char *name = (char *) arg;
    while(1)
    {
        sem_wait(&plate);
        pthread_mutex_lock(&mutex);
        appleNum++;
        printf("%s👨🏻往盘子中放了个苹果🍎 [%s]\n", name, plateInfo().c_str());
        pthread_mutex_unlock(&mutex);
        sem_post(&apple);
        sleep(1);
    }
    return nullptr;
}

void *placeOrange(void* arg)
{
    char *name = (char *) arg;
    while (1)
    {
        sem_wait(&plate);
        pthread_mutex_lock(&mutex);
        orangeNum++;
        printf("%s🧑🏻往盘子中放了一个橘子🍊 [%s]\n", name, plateInfo().c_str());
        pthread_mutex_unlock(&mutex);
        sem_post(&orange);
        sleep(1);
    }
    return nullptr;
}

void *eatApple(void *arg)
{
    char *name = (char *) arg;
    while(1)
    {
        sem_wait(&apple);
        pthread_mutex_lock(&mutex);
        appleNum--;
        printf("%s👩🏻从盘子中拿了一个苹果吃🍎 [%s]\n", name, plateInfo().c_str());
        pthread_mutex_unlock(&mutex);
        sem_post(&plate);
    }
    return nullptr;
}

void *eatOrange(void *arg)
{
    char *name = (char *) arg;
    while(1)
    {
        sem_wait(&orange);
        pthread_mutex_lock(&mutex);
        orangeNum--;
        printf("%s😋从盘子中拿了一个橘子吃🍊 [%s]\n", name, plateInfo().c_str());
        pthread_mutex_unlock(&mutex);
        sem_post(&plate);
    }
    return nullptr;
}



int main()
{
    sem_init(&orange, 0, 0);
    sem_init(&apple, 0 , 0);
    sem_init(&plate, 0, 2);
    pthread_t father, mother, son, daughter;

    pthread_create(&father, nullptr, placeApple, (void*)"fahter");
    pthread_create(&mother, nullptr, placeOrange, (void*)"mohter");
    pthread_create(&son, nullptr, eatOrange, (void*)"son");
    pthread_create(&daughter, nullptr, eatApple, (void*)"daughter");

    while(1)
    {
        printf("[main thread]\n");
        sleep(1);
    }
    return 0;
}
