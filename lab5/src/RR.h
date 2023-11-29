#pragma once
#include "job.h"
#include "util.h"

const int RR_PER = 4;

struct job_rr : public job
{
    // int _wait; // 等待的总时间
    int _leftRuntime;
    bool _inRunqueue;
    // bool _isProcessed;
    // int _lastInterruptTime; //上一次被切换走的时刻
    // std::vector<timePeriod> _runPeriod; // 转移到父类中
    job_rr(const job&);
};

class RR
{
private:
    std::vector<job_rr> _jobs;
    std::queue<job_rr*> _runqueue;
    std::vector<job_rr*> _schedulingList;
    int _totalTime; //总周转时间
    double _totalTime_with_weight; //总带权周转时间

private:
    void solveTimeRecord();
    void schedulingInfo();
    bool allInQueue();
    // test for info redirect to python
    void infoForPy();



public:
    RR(const std::vector<job>&);
    timeRecord run(bool);
    void outputSchedulingInfo();
};

