#include <iostream>
#include <cstdlib>
#include <queue>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string>
#include <cstring>
#include <cassert>

int TIME_TO_SLEEP_PRO = 1;
int TIME_TO_SLEEP_CUM = 1;

// const int CAPACITY = 10;
sem_t mutex;
sem_t empty;
int productID = 0;

class productQueue
{
private:
    std::queue<int> _queue;

public:
    productQueue() {}
    productQueue(const productQueue &) = delete;
    productQueue &operator=(const productQueue &) = delete;

    // bool isFull()
    // {
    //     return _queue.size() == CAPACITY;
    // }
    // bool isEmpty()
    // {
    //     return _queue.empty();
    // }

    void push(const std::string &name)
    {
        sem_wait(&mutex);
        sem_wait(&empty);
        int ret = productID;
        productID++;
        _queue.push(ret);
        printf("%s 生产产品编号为:%d的商品\n", name.c_str(), ret);
        sem_post(&mutex);
    }

    void pop(const std::string &name)
    {
        sem_wait(&mutex);
        if (_queue.empty())
        {
            sem_post(&mutex);
            return;
        }
        int ret = _queue.front();
        _queue.pop();
        printf("%s 消费产品编号为:%d的商品\n", name.c_str(), ret);
        sem_post(&mutex);
        sem_post(&empty);
    }
};

productQueue product_queue;

void *produce(void *args)
{
    while (1)
    {
        char *name = (char *)args;
        // printf("%s\n", name);
        product_queue.push(name);
        // if(product == -1)   return nullptr;

        // std::cout << name << " 生产产品编号为" << product << "的产品"<< std::endl;
        //  printf("%s 生产产品编号为:%d的商品\n", name, product);
        sleep(TIME_TO_SLEEP_PRO);
    }
    return nullptr;
}

void *consume(void *args)
{
    while (1)
    {
        char *name = (char *)args;
        // printf("%s\n", name);
        product_queue.pop(name);
        sleep(TIME_TO_SLEEP_CUM);
    }

    return nullptr;
}

void usage()
{
    std::cout << "usage:" << std::endl;
    std::cout << "\t\t./[name of program] [time for producer to sleep] [time for consumer to sleep]" << std::endl;
    std::cout << "\t\t./[name of program] with no para, the default sleeping time is 1s" << std::endl;
}

void parmProcess(int argc, char *argv[])
{
    if (argc != 1 && argc != 3)
    {
        usage();
        exit(-1);
    }
    if (argc > 1)
    {
        if (strcmp(argv[1], "-h") == 0)
        {
            usage();
            exit(0);
        }
    }
    if (argc == 3)
    {
        int proTime = atoi(argv[1]);
        int cumTime = atoi(argv[2]);
        if (proTime <= 0 || proTime > 20 || cumTime <= 0 || cumTime > 20)
        {
            std::cout << "please input a valid time [1, 20]" << std::endl;
            usage();
            exit(-1);
        }
        else
        {
            TIME_TO_SLEEP_PRO = proTime;
            TIME_TO_SLEEP_CUM = cumTime;
        }
    }
}

// ./[] 1 1 or ./[]

int main(int argc, char *argv[])
{
    parmProcess(argc, argv);

    pthread_t producer1, producer2, producer3;
    pthread_t consumer1, consumer2;

    sem_init(&mutex, 0, 1);
    sem_init(&empty, 0, 10);
    // sem_init(&mutex_ID, 0, 1);
    pthread_create(&producer1, nullptr, produce, (void *)"一号生产者🤖");
    pthread_create(&producer2, nullptr, produce, (void *)"二号生产者🤖");
    pthread_create(&producer3, nullptr, produce, (void *)"三号生产者🤖");

    pthread_create(&consumer1, nullptr, consume, (void *)"一号消费者😎");
    pthread_create(&consumer2, nullptr, consume, (void *)"二号消费者😎");

    while (1)
    {
        printf("main thread\n");
        sleep(1);
    }
    return 0;
}
