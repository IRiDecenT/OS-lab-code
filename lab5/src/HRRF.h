#pragma once
#include "job.h"
#include "utils.h"

struct job_hrrf : public job
{
    bool _isProcessed;
    int _waitTime;
    double _respones;
    job_hrrf(const job& j);
};

class HRRF
{
private:
    std::vector<job_hrrf> _jobs;
    std::vector<job_hrrf> _runqueue; //存放调度结果
    //std::queue<job_hrrf> _waitqueue;
    int _totalTime; //总周转时间
    double _totalTime_with_weight; //总带权周转时间

private:
    void schedulingInfo();
    void infoForPy();


public:
    HRRF(const std::vector<job>&);
    timeRecord run(bool);
    void outputSchedulingInfo();

};

auto compByRespones = [](job_hrrf* j1, job_hrrf* j2)
{   return j1->_respones < j2->_respones; };

