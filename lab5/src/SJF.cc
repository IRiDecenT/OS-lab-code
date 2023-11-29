#include "SJF.h"

SJF::SJF(const std::vector<job>& v) : _totalTime(0), _totalTime_with_weight(0.0)
{
    _jobs.reserve(v.size());
    for (const auto& e : v)
        _jobs.push_back(job_sjf(e));

    std::sort(_jobs.begin(), _jobs.end(), JobComp);
    // ******DEBUG*******
    /*for(const auto& e : _jobs)
       std::cout << e;*/
}

void SJF::schedulingInfo() {
    std::cout << "模拟SJF调度, 进程调度执行的顺序如下:" << std::endl;
    for (const auto& j : _runqueue)
        std::cout << *j;
    printTime({ (double)_totalTime / _jobs.size(), _totalTime_with_weight / _jobs.size()});
}

void SJF::infoForPy()
{
    int sz = _jobs.size();
    std::cout<< ":SJF|" << (double)_totalTime / sz << "|" << _totalTime_with_weight / sz << std::endl;
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

timeRecord SJF::run(bool flag) {
    if (!_jobs.empty()) {
        _runqueue.push_back(&_jobs[0]);

    _jobs[0]._isProcessed = true;

    int curTime = _jobs[0].arrivalTime() + _jobs[0].runTime();
    _jobs[0].getRunPeriod().push_back({_jobs[0].arrivalTime(),
                                        _jobs[0].arrivalTime() + _jobs[0].runTime()});
    _totalTime += _jobs[0].runTime();
    _totalTime_with_weight += 1;
    while (_runqueue.size() != _jobs.size())
    {
        std::priority_queue<job_sjf*,
            std::vector<job_sjf*>,
            decltype(compByRunTime)> waitqueue(compByRunTime);
        for (int i = 0; i < _jobs.size(); ++i)
            if (!_jobs[i]._isProcessed && _jobs[i].arrivalTime() < curTime)
                waitqueue.push(&_jobs[i]);

        if (waitqueue.empty())
        {
            for (auto& e : _jobs)
            {
                if (!e._isProcessed)
                {
                    waitqueue.push(&e);
                }
            }
        }
        job_sjf* curJob = waitqueue.top();
        curJob->_isProcessed = true;
        curTime += curJob->runTime();
        curJob->getRunPeriod().push_back({curTime - curJob->runTime(), curTime});
        _totalTime += curTime - curJob->arrivalTime();
        _totalTime_with_weight += ((curTime - curJob->arrivalTime()) / (1.0 * curJob->runTime()));
        _runqueue.push_back(curJob);

    }
}
    if(flag)
        infoForPy();
    return { (double)_totalTime / _jobs.size(), _totalTime_with_weight / _jobs.size()};
}

void SJF::outputSchedulingInfo()
{
    schedulingInfo();
}

job_sjf::job_sjf(const job& j)
    : job(j),
    _isProcessed(false),
    remainingTime(j.runTime()) {}

