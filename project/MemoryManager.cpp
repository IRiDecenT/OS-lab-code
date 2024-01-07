#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include <map>
#include <bitset>
#include <mutex>
#include <fstream>
#include <memory.h>
#include <thread>
#include <cmath>
#include <random>
#include <chrono>
#include <list>
#include <unordered_map>

// 把两种替换算法通过继承虚基类的方式实现, 两种算法的实现都是通过重写虚函数实现的，
// 只需要重写replace函数即可，返回replace的页号
// FIFO: 利用队列实现，每次淘汰队首元素
// LRU: 利用hash表 + 双向链表实现，每次淘汰链表尾部元素
// 只需要在process类中添加一个成员变量，指向虚基类的指针即可，通过多态的方式实现

int REPLACE_FLAG = 0;
std::string REPLACE_NAME = "FIFO";
namespace MEM{
    const int MEM_SIZE = 1024 * 16;
    const int PAGE_SIZE = 256;
    const int ALLOC_PAGES_PER_PROCESS = 9;
    const int PROCESS_NUM = 12;
    const int REAL_PAGES = MEM_SIZE / PAGE_SIZE;
    const int VIRTUAL_PAGES = 15;
    const int FIFO_REPLACE = 0;
    const int LRU_REPLACE = 1;
};

// 位图 指定页面是否被分配
std::bitset<MEM::MEM_SIZE / MEM::PAGE_SIZE> PageBitmap;

namespace MEM{

    class ReplaceAlgo
    {
    public:
        virtual int replacePage()=0; 
        // push的是物理页号
        virtual void push(int page)=0;
        virtual ~ReplaceAlgo(){}
    };

    class FIFO : public ReplaceAlgo
    {
    private:
        std::queue<int> _queue;
    public:
        virtual int replacePage()
        {
            int replace = _queue.front();
            _queue.pop();
            _queue.push(replace);
            return replace;
        }
        virtual void push(int page)
        {
            _queue.push(page);
        }
    };

    class LRU : public ReplaceAlgo
    {
    private:
        std::unordered_map<int, std::list<int>::iterator> _hashTable;
        std::list<int> _pageList;
    public:
        // 一定时缺页时发生替换！
        virtual int replacePage()
        {
            int replace = _pageList.back();
            _pageList.pop_back();
            _pageList.push_front(replace);
            // 删除时存在迭代器失效，需要把hash表里存的迭代器信息更新
            _hashTable[replace] = _pageList.begin();
            return replace;
        }
        virtual void push(int page)
        {
            _pageList.push_front(page);
            // 插入时list没有迭代器失效的问题
            _hashTable.insert({page, _pageList.begin()});
        }
        // LRU需要记录历史访问页的次数，如果不缺页，需要更新list中的页排布关系
        // 因为每次替换的都是list尾部的页，而如果一个被重复访问的页，需要把他放到list头并更新hash中的迭代器
        // 需要传物理页号
        void accessUpdate(int accessPhysicalPage)
        {
            auto it = _hashTable[accessPhysicalPage];
            _pageList.erase(it);
            _pageList.push_front(accessPhysicalPage);
            _hashTable[accessPhysicalPage] = _pageList.begin();
        }
    };

    struct PageTableEntity
    {
        int _frameNum; // 物理页号
        bool _valid;   // 有效位
    };

    class Process
    {
    private:
        int _processID;
        int _page_faults;
        std::vector<PageTableEntity> _page_table;
        std::queue<int> _freePageQueue;
        // std::queue<int> _loadedPageQueue;
        ReplaceAlgo* replaceAlgo;
    public:
        Process(int processID)
            : _processID(processID),
            _page_faults(0),
            _page_table(REAL_PAGES, {-1, false})
        {
            if(REPLACE_FLAG == FIFO_REPLACE)
            {
                replaceAlgo = new FIFO;
            }
            else if(REPLACE_FLAG == LRU_REPLACE)
            {
                replaceAlgo = new LRU;
            }
            else
            {
                printf("[进程%d DEBUG] unknown FLAG\n", _processID);
                exit(-1);
            }
        }

        ReplaceAlgo* getReplaceAlgoPtr()
        {
            return replaceAlgo;
        }

        void printPageTable()
        {
            printf("[进程%d DEBUG] 页表信息: \n", _processID);
            for(int i = 0; i < _page_table.size(); ++i)
            {
                printf("[进程%d DEBUG] 虚拟页号: %d, 物理页号: %d, 有效位: %d\n",
                        _processID, i, _page_table[i]._frameNum, _page_table[i]._valid);
            }
        }

        int getPID() const
        {
            return _processID;
        }
        void addPageTableEntity(int virPage)
        {
            if(_freePageQueue.empty())
            {

                int replacedPage = replaceAlgo->replacePage();
                int formerPage = 0;
                for(int i = 0; i < _page_table.size(); i++)
                {
                    auto& e = _page_table[i]; 
                    if(e._frameNum == replacedPage)
                    {
                        formerPage = i;
                        e._valid = false;
                        e._frameNum = -1;
                        break;
                    }
                }
                printf("[进程%d DEBUG %s] 发生了页面替换 虚拟页把%d替换为%d \n", _processID, REPLACE_NAME.c_str(), formerPage, virPage);
                _page_table[virPage]._frameNum = replacedPage;
                _page_table[virPage]._valid = true;
            }
            else
            {
                int PhysicPage = _freePageQueue.front();
                _page_table[virPage]._frameNum = PhysicPage;
                _page_table[virPage]._valid = true;
                _freePageQueue.pop();
                replaceAlgo->push(PhysicPage);
            }

        }

        void addPage(int page)
        {
            _freePageQueue.push(page);
        }

        void freePageTable()
        {
            for(auto& e : _page_table)
            {
                PageBitmap[e._frameNum] = false;
                e._valid = false;
            }
        }

        bool isPageFault(int virPage)
        {
            return !((_page_table[virPage])._valid);
        }

        int getPhysicalPage(int virPage)
        {
            return (_page_table[virPage])._frameNum;
        }
        void addPageFault()
        {
            ++_page_faults;
        }
        void printFaults()
        {
            printf("[进程%d DEBUG %s] 缺页次数: %d, 缺页中断率为: %f\n", _processID, REPLACE_NAME.c_str(), _page_faults, _page_faults / 200.0);
        }
    };


    class MemoryManager
    {
    private:
        int _mem_size;
        int _page_size;
        int _free_pages;
        // 内存资源 临界资源
        char* Memory;
        std::mutex _mutex;
    public:
        MemoryManager(int mem_size = MEM_SIZE, int page_size = PAGE_SIZE)
            : _mem_size(mem_size), _page_size(page_size), _free_pages(mem_size / page_size)
        {
            Memory = new char[MEM::MEM_SIZE];
        }

        ~MemoryManager()
        {
            delete[] Memory;
            Memory = nullptr;
        }

        bool allocPage(Process& process)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            // 当前内存空闲页小于10时
            if(_free_pages < ALLOC_PAGES_PER_PROCESS + 1)
                return false;
            // 分配10个页面
            for(int i = 0; i < ALLOC_PAGES_PER_PROCESS + 1; ++i)
            {
                // 在位图中找第一个未分配的页面
                for(int freepage = 0; freepage < PageBitmap.size(); ++freepage)
                {
                    if(!PageBitmap[freepage])
                    {
                        // process.addPageTableEntity(freepage);
                        --_free_pages;
                        process.addPage(freepage);
                        PageBitmap[freepage] = true;
                        break;
                    }
                }
            }
            // process.printPageTable();
            return true;
        }

        bool free(Process& process)
        {
            std::lock_guard<std::mutex> lock(_mutex);
            process.freePageTable();
            _free_pages += (ALLOC_PAGES_PER_PROCESS + 1);
            return true;
        }

        // 传递自己封装的进程对象和虚拟页号以及页内偏移量
        void accessMemory(Process& process, int virPage, int offset)
        {
            if(process.isPageFault(virPage))
            {
                printf("[进程%d DEBUG %s] 虚拟页: %d 产生缺页中断\n", process.getPID(), REPLACE_NAME.c_str(), virPage);
                process.addPageFault();
                loadPage(process, virPage);
            }
            // process.printPageTable();
            // 访问内存
            int physicalPage = process.getPhysicalPage(virPage);
            int physicalAddr = physicalPage * _page_size + offset;
            // 特别对于LRU算法需要记录之前访问的历史，在实现中即更新list和hash表
            if(REPLACE_FLAG == LRU_REPLACE)
            {
                LRU* ptr = dynamic_cast<LRU*>(process.getReplaceAlgoPtr());
                ptr->accessUpdate(physicalPage);
            }
            printf("[进程%d DEBUG %s] 模拟内存内存基址: %p, 访问内存地址: %p\n",process.getPID(), REPLACE_NAME.c_str(), Memory, Memory + physicalAddr);
            printf("[进程%d DEBUG %s] 访问虚拟页号:%d, 页面内偏移:%d 对应的物理页号: %d\n",process.getPID(), REPLACE_NAME.c_str(), virPage, offset, physicalPage);
            printf("[进程%d DEBUG %s] 模拟情况下的虚拟地址: %s, 物理地址: %s, 访问内存: %c\n",
                    process.getPID(),
                    REPLACE_NAME.c_str(),
                    getAddr(virPage, offset).c_str(),
                    getAddr(physicalPage, offset).c_str(),
                    Memory[physicalAddr]);
            //printf("[进程%d DEBUG] 访问内存: %c\n", process.getPID(), Memory[physicalAddr]);

        }
        std::string getAddr(int pageNum, int offset)
        {
            std::bitset<6> bin_pageNum(pageNum);
            std::bitset<8> bin_offset(offset);
            return  bin_pageNum.to_string() + bin_offset.to_string();
        }

        void loadPage(Process& process, int virPage)
        {
            std::string filename = "job_" + std::to_string(process.getPID()) + ".txt";
            std::ifstream fin(filename);
            if(!fin.is_open())
            {
                printf("[进程%d DEBUG] 文件打开失败\n", process.getPID());
                exit(-1);
            }
            process.addPageTableEntity(virPage);
            // 按照字节读取文件
            std::vector<char> buffer(_page_size);
            fin.seekg(virPage * _page_size);
            // 读取256字节
            fin.read(buffer.data(), _page_size);
            // 将读取的数据写入模拟内存
            int physicalPage = process.getPhysicalPage(virPage);
            int physicalAddr = physicalPage * _page_size;
            memcpy(Memory + physicalAddr, buffer.data(), _page_size);
            char *ptr = Memory + physicalAddr;
            // for(int i = 0; i < _page_size; ++i)
            // {
            //     printf("%c", ptr[i]);
            // }
            // printf("\n");
        }
    };

    double pageDistribution(int i)
    {
        return 1.0 / std::sqrt(i + 1);
    }
}

MEM::MemoryManager manager;
int main(int argc, char* argv[])
{
    if(argc > 1)
    {
        int op = atoi(argv[1]);
        if(op != 0 && op != 1)
        {
            printf("invalid args! 0: FIFO 1: LRU\n");
            exit(-1);
        }
        else
        {
            REPLACE_FLAG = op;
            if(REPLACE_FLAG == MEM::FIFO_REPLACE)
                REPLACE_NAME = "FIFO";
            else if(REPLACE_FLAG == MEM::LRU_REPLACE)
                REPLACE_NAME = "LRU";
            else
            {
                printf("unknown flag\n");
                exit(-1);
            }
        }
    }


    // 使用 random_device 获取真实的随机设备
    std::random_device rd;
    // 使用 mt19937 作为伪随机数生成器，并用 random_device 的种子初始化
    std::mt19937 gen(rd());
    std::vector<double> probabilities;
    // 使用自定义的概率分布
    for (int i = 0; i < MEM::VIRTUAL_PAGES; ++i) {
        probabilities.push_back(MEM::pageDistribution(i));
    }
    // 构造离散概率分布
    std::discrete_distribution<int> distribution(probabilities.begin(), probabilities.end());


    std::vector<MEM::Process> processes;
    for(int i = 0; i < MEM::PROCESS_NUM; ++i)
    {
        processes.push_back(MEM::Process(i));
    }
    std::vector<std::thread> threads;
    for(int i = 0; i < MEM::PROCESS_NUM; ++i)
    {
        threads.push_back(std::thread([&processes, i, &distribution, &gen](){
            bool flag = false;
            do
            {
                flag = manager.allocPage(processes[i]);
            } while (!flag);
            
            for(int j = 0; j < 200; ++j)
            {
                int virPage = distribution(gen);
                // int virPage = rand() % MEM::REAL_PAGES;
                int offset = rand() % MEM::PAGE_SIZE;
                manager.accessMemory(processes[i], virPage, offset);

                // 生成随机的休眠时间（0-100ms）
                std::chrono::milliseconds sleepTime(std::rand() % 101);
                // 休眠指定的时间
                std::this_thread::sleep_for(sleepTime);
            }
            processes[i].printFaults();
            manager.free(processes[i]);
        }));
    }
    for(auto& t : threads)
    {
        t.join();
    }
    for(int i = 0; i < MEM::PROCESS_NUM; ++i)
    {
        processes[i].printFaults();
    }
    return 0;
}
