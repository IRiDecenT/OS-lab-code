#include "SRTF.h"

SRTF::SRTF(const std::vector<job>& v) : _totalTime(0), _totalTime_with_weight(0.0) {
    _jobs.reserve(v.size());
    for (const auto& e : v)
        _jobs.push_back(job_srtf(e));

    std::sort(_jobs.begin(), _jobs.end(), JobComp);
    //******DEBUG*******
    /*for(const auto& e : _jobs)
       std::cout << e;*/
}

void SRTF::infoForPy()
{
    int sz = _jobs.size();
    std::cout<< ":SRTF|" << (double)_totalTime / sz << "|" << _totalTime_with_weight / sz << std::endl;
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

bool SRTF::allFinished()
{
    for(auto& j : _jobs)
        if(j._state != _FINISHED)
            return false;
    return true;
}

void SRTF::schedulingInfo() {
    std::cout << "模拟SRTF调度, 进程的调度执行顺序如下:" << std::endl;
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
}

timeRecord SRTF::run(bool flag) {
    auto curJob = &_jobs[0];
    curJob->_state = _RUNNING;
    // curTime定义为当前job的开始运行的时间
    int curTime = curJob->arrivalTime();
    _schedulingList.push_back(curJob);
    while(!allFinished())
    {
        int end = curTime + curJob->runTime();
        std::priority_queue<job_srtf*,
                        std::vector<job_srtf*>,
                        decltype(srtfCompByRemaining)> _waitqueue(srtfCompByRemaining);
        for(auto& j : _jobs)
        {
            if( j._state != _FINISHED && j._state != _RUNNING && j.arrivalTime() < end)
                _waitqueue.push(&j);
        }
        job_srtf* nextJob_ptr;
        if(_waitqueue.empty())
        {
            for(auto& j : _jobs)
                if(j._state != _RUNNING && j._state != _RUNNING)
                {
                    nextJob_ptr = &j;
                    break;
                }
        }
        else
            nextJob_ptr = _waitqueue.top();
        if(nextJob_ptr->runTime() < end - nextJob_ptr->arrivalTime())
        {
            curJob->remainingTime -= (nextJob_ptr->arrivalTime() - curTime);
            curJob->_state = _UNFINISHED;
            curJob->getRunPeriod().push_back({curTime, nextJob_ptr->arrivalTime()});

            curJob = nextJob_ptr;
            curTime = nextJob_ptr->arrivalTime();
            curJob->_state = _RUNNING;
        }
        else
        {
            curJob->remainingTime = 0;
            curJob->_state = _FINISHED;
            curJob->getRunPeriod().push_back({curTime, curTime + curJob->runTime()});

            curJob = nextJob_ptr;
            if(_waitqueue.empty())
                curTime = nextJob_ptr->arrivalTime();
            else
                curTime = end;
            curJob->_state = _RUNNING;
        }

    }


    return { (double)_totalTime / _jobs.size(), _totalTime_with_weight / _jobs.size() };
}

void SRTF::outputSchedulingInfo()
{
    schedulingInfo();
}

job_srtf::job_srtf(const job& j)
    : job(j),
    _state(_UNFINISHED),
    remainingTime(j.runTime()) {}

