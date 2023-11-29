#include "FCFS.h"

FCFS::FCFS(const std::vector<job> &v):
        _runqueue(v),
        _totalTime(0),
        _totalTime_with_weight(0.0)
{}

void FCFS::schedulingInfo()
{
    std::cout<< "模拟FCFS调度算法, 进程的调度执行顺序如下:" << std::endl;
    for(const auto& j : _runqueue)
        std::cout<< j;
    int sz = _runqueue.size();
    printTime({ (double)_totalTime / sz, _totalTime_with_weight / sz } );
}

void FCFS::infoForPy()
{
    int sz = _runqueue.size();
    std::cout<< ":FCFS|" << (double)_totalTime / sz << "|" << _totalTime_with_weight / sz << std::endl;
    for(const auto& j : _runqueue)
    {
        std::cout << j.name() << ":" << j.arrivalTime();
        for(const auto& period : j.getRunPeriod())
        {
            std::cout << "|" << period.first << "-"
                    << period.second;
        }
        std::cout << std::endl;
    }
}

// void FCFS::printTime()
// {
//     std::cout<<"平均周转时间:" << " " << (double)_totalTime / _runqueue.size() << std::endl;
//     std::cout<<"平均带权周转时间:" << " " << _totalTime_with_weight / _runqueue.size() << std::endl;
// }

timeRecord FCFS::run(bool isVisualized)
{
    int sz = _runqueue.size();
    std::sort(_runqueue.begin(), _runqueue.end(), compByArrivalFirst());
    int preEnd = _runqueue.front().arrivalTime();
    for(auto& j : _runqueue)
    {
        j.getRunPeriod().push_back({preEnd, preEnd + j.runTime()});
        int wait = ( preEnd <= j.arrivalTime() ? 0 : preEnd - j.arrivalTime() );
        _totalTime += ( wait + j.runTime() );
        _totalTime_with_weight += ( wait + j.runTime()) / (1.0 * j.runTime());
        // for debug
        //std::cout<< _totalTime << " " << _totalTime_with_weight<< std::endl;
        preEnd = j.arrivalTime() + wait + j.runTime();
    }
    //schedulingInfo();
    //printTime();
    //return std::make_pair((double)_totalTime / sz, _totalTime_with_weight / sz)
    if(isVisualized)
        infoForPy();
    return { (double)_totalTime / sz, _totalTime_with_weight / sz };
}

void FCFS::outputSchedulingInfo()
{
    schedulingInfo();
}
