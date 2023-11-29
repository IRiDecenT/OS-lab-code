#pragma once

#include "utils.h"
#include "job.h"

struct job_sjf : public job
{
    bool _isProcessed;
    int remainingTime;
    job_sjf(const job& j);
};

class SJF {
private:

    std::vector<job_sjf> _jobs;
    std::vector<job_sjf*> _runqueue;

    int _totalTime;
    double _totalTime_with_weight;
    void schedulingInfo();
    void infoForPy();

public:
    SJF(const std::vector<job>& v);
    timeRecord run(bool);
    void outputSchedulingInfo();

};
auto compByRemaining = [](job_sjf* j1, job_sjf* j2)
{   return j1->remainingTime > j2->remainingTime; };

auto compByRunTime = [](job_sjf* j1, job_sjf* j2)
{   return j1->runTime() > j2->runTime(); };
