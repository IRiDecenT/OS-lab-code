#include "HRRF.h"

HRRF::HRRF(const std::vector<job> & v):_totalTime(0), _totalTime_with_weight(0.0)
{
    _jobs.reserve(v.size());

    for(const auto& e : v)
        _jobs.push_back(job_hrrf(e));

    // for debug
    // for(const auto& e : _jobs)
    //     std::cout << e;

    std::sort(_jobs.begin(), _jobs.end(),
            [](job_hrrf& j1, job_hrrf& j2){ return j1.arrivalTime() < j2.arrivalTime();});
    //print();

    // for debug
    // for(const auto& e : _jobs)
    //     std::cout << e;
}

void HRRF::schedulingInfo()
{
    std::cout << "模拟HRRF调度, 进程的调度执行顺序如下:" << std::endl;
    for(const auto& j : _runqueue)
        std::cout<< j;
    printTime({ (double)_totalTime / _jobs.size(), _totalTime_with_weight / _jobs.size() });
}

void HRRF::infoForPy()
{
    int sz = _jobs.size();
    std::cout<< ":HRRF|" << (double)_totalTime / sz << "|" << _totalTime_with_weight / sz << std::endl;
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

timeRecord HRRF::run(bool isVisualized)
{
    _runqueue.push_back(_jobs[0]);
    _jobs[0]._isProcessed = true;
    int curTime = _jobs[0].arrivalTime() + _jobs[0].runTime();

    _jobs[0].getRunPeriod().push_back({_jobs[0].arrivalTime(), curTime});

    _totalTime += _jobs[0].runTime();
    _totalTime_with_weight += 1;
    while(_runqueue.size() != _jobs.size())
    {
        // std::cout << curTime << std::endl; // for debug
        std::priority_queue<job_hrrf*,
                            std::vector<job_hrrf*>,
                            decltype(compByRespones)> waitqueue(compByRespones);
        for(int i = 0; i < _jobs.size(); ++i)
        {
            if( !_jobs[i]._isProcessed && _jobs[i].arrivalTime() < curTime)
            {
                _jobs[i]._waitTime = (curTime - _jobs[i].arrivalTime());
                _jobs[i]._respones = (double)_jobs[i]._waitTime / _jobs[i].runTime();
                // for debug
                // std::cout << _jobs[i].name() << " " << _jobs[i]._waitTime << " " << _jobs[i]._respones << std::endl;
                waitqueue.push(&_jobs[i]);
            }
        }
        if(waitqueue.empty())
        {
            for(auto& e : _jobs)
                if(!e._isProcessed)
                    waitqueue.push(&e);
        }
        job_hrrf* curJob = waitqueue.top();
        curJob->_isProcessed = true;
        curTime = curJob->arrivalTime() + curJob->_waitTime + curJob->runTime();
        curJob->getRunPeriod().push_back({curJob->arrivalTime() + curJob->_waitTime, curTime});
        _totalTime += (curJob->_waitTime + curJob->runTime());
        _totalTime_with_weight += (curJob->_waitTime + curJob->runTime()) / (1.0 * curJob->runTime());
        _runqueue.push_back(*curJob);
    }

    if(isVisualized)
        infoForPy();
    //schedulingInfo();
    return { (double)_totalTime / _jobs.size(), _totalTime_with_weight / _jobs.size() };
}

void HRRF::outputSchedulingInfo()
{
    schedulingInfo();
}

job_hrrf::job_hrrf(const job &j)
            : job(j),
            _isProcessed(false),
            _waitTime(0),
            _respones(0.0){}
