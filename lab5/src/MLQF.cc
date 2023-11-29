#include "MLQF.h"


int pri2period(int priority)
{
    switch (priority)
    {
    case PRI0:
        return PRI0_PERIOD;
    case PRI1:
        return PRI1_PERIOD;
    case PRI2:
        return PRI2_PERIOD;
    default:
        std::cerr << "unknow priority" << std::endl;
        exit(-1);
    }
}

void MLQF::solveTimeRecord()
{
    //int size = _jobs.size();
    for(auto& j : _jobs)
    {
        int n = j.getRunPeriod().size();
        int period = j.getRunPeriod()[n - 1].second - j.arrivalTime();
        _totalTime += period;
        _totalTime_with_weight += (double)period / j.runTime();
    }
    //return {(double)_totalTime / size, _totalTime_with_weight / size};
}

void MLQF::infoForPy()
{
    int sz = _jobs.size();
    std::cout<< ":MLQF|" << (double)_totalTime / sz << "|" << _totalTime_with_weight / sz << std::endl;
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

job_mlqf::job_mlqf(const job &j)
    :job(j),
    _leftRuntime(j.runTime()),
    _queueIndex(NO_INQUEUE)
{}

bool MLQF::queuesAllEmpty()
{
    return _queues[PRI0].empty() && _queues[PRI1].empty() && _queues[PRI2].empty();
}

void MLQF::queuePRI0_scheduling(int& curTime)
{
    while(!_queues[PRI0].empty())
    {
        job_mlqf* curJob_ptr = _queues[PRI0].front();
        _queues[PRI0].pop();
        _schedulingList.push_back(curJob_ptr);
        int runtime = pri2period(PRI0);
        if(curJob_ptr->_leftRuntime <= runtime)
        {
            runtime = curJob_ptr->_leftRuntime;
            curJob_ptr->_leftRuntime = 0;
        }
        else
            curJob_ptr->_leftRuntime -= runtime;

        // 记录运行时间片
        (curJob_ptr->getRunPeriod()).push_back({curTime, curTime + runtime});
        // 更新当前时间
        curTime += runtime;
        // 如果当前进程未在时间段内完成，则进入PRI1队列
        if(curJob_ptr->_leftRuntime > 0)
        {
            // 修改当前进程所属队列的信息
            curJob_ptr->_queueIndex = PRI1;
            _queues[PRI1].push(curJob_ptr);
        }
        // 如果当前进程在时间段内完成，则不再进入队列
        // 前面已经pop过了，不需要再pop，但是需要修改所在队列信息（之前没改造成死循环）
        else
            curJob_ptr->_queueIndex = FINISHED;
        // 将当前时间之前到达的任务放入PRI0队列
        for(auto& j : _jobs)
        {
            if(j._queueIndex == NO_INQUEUE && j.arrivalTime() <= curTime)
                _queues[PRI0].push(&j);
        }
    }
}
void MLQF::queuePRI1_scheduling(int& curTime)
{
    // 此时PRI0队列一定为空
    while(!_queues[PRI1].empty())
    {
        job_mlqf* curJob_ptr = _queues[PRI1].front();
        _queues[PRI1].pop();
        _schedulingList.push_back(curJob_ptr);
        int runtime = pri2period(PRI1);
        if(curJob_ptr->_leftRuntime <= runtime)
        {
            runtime = curJob_ptr->_leftRuntime;
            curJob_ptr->_leftRuntime = 0;
        }
        else
            curJob_ptr->_leftRuntime -= runtime;

        // 记录运行时间片
        (curJob_ptr->getRunPeriod()).push_back({curTime, curTime + runtime});
        // 更新当前时间
        curTime += runtime;
        // 如果当前进程未在时间段内完成，则进入PRI2队列
        if(curJob_ptr->_leftRuntime > 0)
        {
            curJob_ptr->_queueIndex = PRI2;
            _queues[PRI2].push(curJob_ptr);
        }
        else
            curJob_ptr->_queueIndex = FINISHED;
        // 此处检查是否有新任务来
        // 对于新任务，需要放进PRI0队列，然后调用queuePRI0_scheduling先处理新任务
        for(auto& j : _jobs)
        {
            if(j._queueIndex == NO_INQUEUE && j.arrivalTime() < curTime)
                _queues[PRI0].push(&j);
        }
        // 处理优先级更高的新任务（如果有）
        queuePRI0_scheduling(curTime);
    }
}

void MLQF::queuePRI2_scheduling(int& curTime)
{
    // 此处PRI0和PRI1队列一定为空
    while(!_queues[PRI2].empty())
    {
        job_mlqf* curJob_ptr = _queues[PRI2].front();
        _queues[PRI2].pop();
        _schedulingList.push_back(curJob_ptr);
        int runtime = pri2period(PRI2);
        if(curJob_ptr->_leftRuntime <= runtime)
        {
            runtime = curJob_ptr->_leftRuntime;
            curJob_ptr->_leftRuntime = 0;
        }
        else
            curJob_ptr->_leftRuntime -= runtime;
        // 记录运行时间片
        (curJob_ptr->getRunPeriod()).push_back({curTime, curTime + runtime});
        // 更新当前时间
        curTime += runtime;
        // 如果当前进程未在时间段内完成，则重新进入PRI2队列等待被再次调度
        if(curJob_ptr->_leftRuntime > 0)
            _queues[PRI2].push(curJob_ptr);
        else
            curJob_ptr->_queueIndex = FINISHED;
        // 此处检查是否有新任务来
        // 对于新任务，需要放进PRI0队列，然后调用queuePRI0_scheduling先处理新任务
        for(auto& j : _jobs)
        {
            if(j._queueIndex == NO_INQUEUE && j.arrivalTime() < curTime)
                _queues[PRI0].push(&j);
        }
        // 处理优先级更高的新任务（如果有）
        queuePRI0_scheduling(curTime);
        queuePRI1_scheduling(curTime);
    }
}

void MLQF::schedulingInfo()
{
    std::cout << "模拟多级反馈队列调度(非抢占式MLQF), 三级队列, 进程的调度执行顺序如下:" << std::endl;
    for(const auto& ptr : _schedulingList)
        std::cout << ptr->name() << " ";
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
    // 输出平均周转时间和平均加权周转时间
    printTime({ (double)_totalTime / _jobs.size(), _totalTime_with_weight / _jobs.size()});
}

MLQF::MLQF(std::vector<job> &jobs, int queue_size)
    : _totalTime(0),
      _totalTime_with_weight(0.0)
{
    _jobs.reserve(jobs.size());
    for(const auto& j : jobs)
        _jobs.push_back(job_mlqf(j));

    std::sort(_jobs.begin(), _jobs.end(), JobComp);

    _queues.reserve(queue_size);
    for(int i = 0; i < queue_size; ++i)
        _queues.push_back(std::queue<job_mlqf*>());

    // for debug
    // for(auto& j : _jobs)
    //     std::cout << j;
}

timeRecord MLQF::run(bool isVisualized)
{
    int curTime = _jobs[0].arrivalTime();
    _jobs[0]._queueIndex = PRI0;
    _queues[PRI0].push(&_jobs[0]);

    while(!queuesAllEmpty())
    {
        queuePRI0_scheduling(curTime); // 调度PRI0优先级队列
        queuePRI1_scheduling(curTime); // 调度PRI1优先级队列
        queuePRI2_scheduling(curTime); // 调度PRI2优先级队列
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
    //schedulingInfo();
    solveTimeRecord();

    if(isVisualized)
        infoForPy();

    return { (double)_totalTime / _jobs.size(), _totalTime_with_weight / _jobs.size()};
}

void MLQF::outputSchedulingInfo()
{
    schedulingInfo();
}
