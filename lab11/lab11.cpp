#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <cassert>

namespace Mem
{
    const int PAGE_SIZE = 1000;
    const int MEM_SIZE = 100000;
    struct Job
    {
        std::string _name;
        int _size;
        // 存放页号索引
        std::vector<int> pageTable;
    };

    class MemoryManage
    {
    private:
        int _memorySize;
        int _pageSize;
        // 位图，表示页面的占用情况 true表示占用，false表示空闲
        std::vector<bool> _bitmap;
        // 分配内存的作业
        std::vector<Job> _jobs;
        // 利用hash表建立作业名和_jobs索引的映射
        std::unordered_map<std::string, int> _jobTable;

    public:
        MemoryManage(int memSize = MEM_SIZE, int pageSize = PAGE_SIZE)
            : _memorySize(memSize), _pageSize(pageSize)
        {
            // 初始化位图
            _bitmap.resize(_memorySize / _pageSize, false);
        }

        void alloc()
        {
            std::string jobName;
            int jobSize;
            std::cout << "请输入作业名：";
            std::cin >> jobName;
            std::cout << "请输入作业所需字节数：";
            std::cin >> jobSize;
            // 计算作业需要占用的页面数 需要考虑到jobSize能否整除_pageSize的问题！
            int pageNum = (jobSize + _pageSize - 1) / _pageSize;
            if (pageNum > freePageCount())
            {
                std::cout << "内存不足以为" << jobName << "分配" << jobSize << "bytes" << std::endl;
                return;
            }
            std::vector<int> allocPages;
            for (int i = 0; i < _bitmap.size() && pageNum > 0; i++)
            {
                if (!_bitmap[i])
                {
                    _bitmap[i] = !_bitmap[i];
                    allocPages.push_back(i);
                    pageNum--;
                }
            }
            Job job = {jobName, jobSize, allocPages};
            _jobs.push_back(job);
            _jobTable[jobName] = _jobs.size() - 1;
            std::cout << "成功为" << jobName << "分配" << jobSize << "bytes" << std::endl;
        }

        void free()
        {
            std::string jobName;
            std::cout << "请输入要回收内存的作业名：";
            std::cin >> jobName;

            auto it = _jobTable.find(jobName);
            if (it == _jobTable.end())
            {
                std::cout << "不存在名为" << jobName << "的作业" << std::endl;
                return;
            }
            // 通过索引找到对应要删除的job
            Job job = _jobs[it->second];
            // 删除一个job,对于在vector中的其他jop，在其后面的需要在哈系表中更新下标关系，vector的删除是移动覆盖
            auto it_update = _jobTable.begin();
            while(it_update!=_jobTable.end())
            {
                if(it_update->second > it->second)
                {
                    (it_update->second)--;
                }
                it_update++;
            }
            for (auto &pageIndex : job.pageTable)
                _bitmap[pageIndex] = false;
            // 从作业表中删除对应的作业
            _jobs.erase(_jobs.begin() + it->second);
            // 删除作业-索引 映射
            _jobTable.erase(it);
            std::cout << "回收作业" << jobName << "成功" << std::endl;
        }
        void displayMemoryAllocation()
        {
            // auto it = _jobTable.begin();
            // while(it != _jobTable.end()){
            //     std::cout << "[DEBUG]" << it->first << it->second << std::endl;
            //     it++;
            // }


            // 显示内存分配情况
            std::cout << "内存分配情况：" << std::endl;

            for (const Job &job : _jobs)
            {
                std::cout << "作业" << job._name << "占用页面数:" << job.pageTable.size() << "\t";
                std::cout << "占用页框号：";
                for (int page : job.pageTable)
                {
                    std::cout << page << " ";
                }
                std::cout << std::endl;
            }

            // 显示空闲区情况
            std::cout << "空闲区情况：" << std::endl;
            int start = -1;
            int length = 0;
            for (int i = 0; i < _bitmap.size(); ++i)
            {
                if (!_bitmap[i])
                {
                    if (start == -1)
                    {
                        start = i;
                        length = 1;
                    }
                    else
                        length++;
                }
                else
                {
                    if (start != -1)
                    {
                        std::cout << "起始地址：" << start * _pageSize << "，长度：" << length * _pageSize << std::endl;
                        start = -1;
                        length = 0;
                    }
                }
            }
            if (start != -1)
                std::cout << "起始地址：" << start * _pageSize << "，长度：" << length * _pageSize << std::endl;
        }

        void accessMemory()
        {
            std::string jobName;
            int logicalAddress;

            std::cout << "请输入作业名：";
            std::cin >> jobName;
            std::cout << "请输入逻辑访问地址（字节偏移）：";
            std::cin >> logicalAddress;
            // 查找作业
            auto it = _jobTable.find(jobName);
            if (it == _jobTable.end())
            {
                std::cout << "找不到名为" << jobName << "的作业。" << std::endl;
                return;
            }
            const Job &job = _jobs[it->second];
            // 计算物理地址
            int pageNum = logicalAddress / _pageSize;
            if (pageNum >= job.pageTable.size())
            {
                std::cout << "错误：越界访问。" << std::endl;
                return;
            }

            int physicalAddress = job.pageTable[pageNum] * _pageSize + (logicalAddress % _pageSize);
            std::cout << "逻辑地址 " << logicalAddress << " 对应的物理地址是 " << physicalAddress << "。" << std::endl;
            std::cout << "对应物理页框号:" << job.pageTable[pageNum] << " " << "页内偏移量为" << logicalAddress % _pageSize << std::endl;
        }

    private:
        int freePageCount()
        {
            // 返回位图中占用情况为false的数量，即空余页面数
            return std::count(_bitmap.begin(), _bitmap.end(), false);
        }
    };
}

int main()
{
    // 初始化内存管理器
    Mem::MemoryManage memoryManage;

    // int fd = open("input.txt", O_WRONLY, 0666);
    // assert(fd > 0);
    // dup2(0, fd);

    int choice;
    do
    {
        // 显示功能菜单
        std::cout << "========== 内存管理功能菜单 ==========" << std::endl;
        std::cout << "0. 退出" << std::endl;
        std::cout << "1. 作业分配内存" << std::endl;
        std::cout << "2. 回收作业内存" << std::endl;
        std::cout << "3. 显示内存分配情况" << std::endl;
        std::cout << "4. 访问" << std::endl;
        std::cout << "请输入您的选择：";
        std::cin >> choice;

        switch (choice)
        {
        case 0:
            std::cout << "感谢使用，再见！" << std::endl;
            break;
        case 1:
            memoryManage.alloc();
            break;
        case 2:
            memoryManage.free();
            break;
        case 3:
            memoryManage.displayMemoryAllocation();
            break;
        case 4:
            memoryManage.accessMemory();
            break;
        default:
            std::cout << "无效的选择，请重新输入。" << std::endl;
        }

    } while (choice != 0);
    return 0;
}