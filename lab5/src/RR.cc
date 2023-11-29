#include "RR.h"

// void RR::updateWait(int curTime)
// {
//     for(auto& j : _jobs)
//         if(!j._isProcessed & j._lastInterruptTime < curTime)
// }

void RR::solveTimeRecord()
{
    //int size = _jobs.size();
    for(const auto& j : _jobs)
    {

        int n = j.getRunPeriod().size();
        int period = j.getRunPeriod()[n - 1].second - j.arrivalTime();
        _totalTime += period;
        _totalTime_with_weight += (double)period / j.runTime();
    }
    //return {(double)_totalTime / size, _totalTime_with_weight / size};
}

void RR::schedulingInfo()
{
    std::cout << "模拟RR调度, 时间片长度为:" << RR_PER <<", 进程的调度执行顺序如下: " << std::endl;
    for(const auto & ptr : _schedulingList)
        std::cout << ptr->name() << " " ;
    std::cout << std::endl;

    std::cout << "每一个进程运行时间片如下:" << std::endl;
    for(const auto& j : _jobs)
    {
        std::cout << j.name() << ": ";
        for(const auto& period : j.getRunPeriod())
        {
            std::cout << "(" << period.first << ", "
                    << period.second << ")" << " ";
        }
        std::cout << std::endl;
    }
    // 平均周转、平均加权周转
    printTime({ (double)_totalTime / _jobs.size(), _totalTime_with_weight / _jobs.size()});
}

bool RR::allInQueue()
{
    bool ret = true;
    for(auto& j : _jobs)
        ret &= j._inRunqueue;
    return ret;
}

// test
void RR::infoForPy()
{
    int sz = _jobs.size();
    std::cout<< ":RR|" << (double)_totalTime / sz << "|" << _totalTime_with_weight / sz << std::endl;
    for(const auto& j : _jobs)
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

RR::RR(const std::vector<job> &jobs)
    : _totalTime(0), _totalTime_with_weight(0.0)
{
    _jobs.reserve(jobs.size());
    for(const auto& j : jobs)
        _jobs.push_back(job_rr(j));

    _schedulingList.reserve(_jobs.size());

    std::sort(_jobs.begin(), _jobs.end(), JobComp);

    // for debug
    // for(auto& e : _jobs)
    //     std::cout<< e;

    // bug 在此处就将所有的任务按照到达时间顺序加入_runqueue的做法并不正确
    // 有可能前面某个进程在执行完一个时间片后加入队列等待比后面某个进程到来的时间要早
    // for(auto& j : _jobs)
    //     _runqueue.push(&j);
}



timeRecord RR::run(bool isVisualized)
{
    _runqueue.push(&_jobs[0]);
    _jobs[0]._inRunqueue = true;
    int curTime = _runqueue.front()->arrivalTime();
    while(!_runqueue.empty())
    {
        job_rr* curJob_ptr = _runqueue.front();
        _runqueue.pop();
        _schedulingList.push_back(curJob_ptr);
        int runtime = RR_PER;
        if(curJob_ptr->_leftRuntime <= RR_PER)
        {
            runtime = curJob_ptr->_leftRuntime;
            curJob_ptr->_leftRuntime = 0;
        }
        else
            curJob_ptr->_leftRuntime -= runtime;

        (curJob_ptr->getRunPeriod()).push_back({curTime, curTime + runtime});
        curTime += runtime;

        // 在当前进程运行时间片结束要被切换时，将在当前时间之到来的进程加入到runqueue中等待
        // 由于_jobs以及按照到达时间排好序按顺序迭代判断即可
        // 存在一个bug: 当前面进程执行完停止调度后过了一段时间才有进程到来情况下
        // 后续的进程虽然前一个条件满但是不满足到达时间小于当前时间，故不会被调度到
        // 需要一段逻辑判断并处理这样的情况
        for(auto& j : _jobs)
        {
            // 此处小于等于因为前一个结束后一个立即到达的情况下默认先处理新来的
            if(!j._inRunqueue && j.arrivalTime() <= curTime)
            {
                _runqueue.push(&j);
                j._inRunqueue = true;
            }
            else
                continue;
        }
        // 当前进程还没处理完，重新加入队列等待再次被调度
        if(curJob_ptr->_leftRuntime != 0)
            _runqueue.push(curJob_ptr);
        // 处理上个bug的逻辑，如果运行队列是空的但是还有未被处理的任务时
        if(_runqueue.empty() && !allInQueue())
        {
            for(auto& j : _jobs)
            {
                if(j._inRunqueue)
                    continue;
                else
                {
                    // 将一段时间后才来的任务加入队列
                    _runqueue.push(&j);
                    // 将当前时间调整为后来任务的到达时间
                    curTime = j.arrivalTime();
                    break;
                }
            }
        }
    }

    // for debug
    // for(auto& ptr : _schedulingList)
    // {
    //     std::cout<<ptr->name()<<std::endl;
    //     for(auto& pair : ptr->_runPeriod)
    //     {
    //         std::cout<< pair.first << " " << pair.second << std::endl;
    //     }
    // }
    // schedulingInfo();
    // infoForPy();
    solveTimeRecord();

    if(isVisualized)
        infoForPy();
    return { (double)_totalTime / _jobs.size(), _totalTime_with_weight / _jobs.size()};
}

void RR::outputSchedulingInfo()
{
    schedulingInfo();
}

job_rr::job_rr(const job & j)
    :job(j)
    ,_leftRuntime(this->runTime())
    ,_inRunqueue(false)
    //_isProcessed(false)
{}
