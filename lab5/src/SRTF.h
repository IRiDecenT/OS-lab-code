#pragma once

#include "utils.h"
#include "job.h"

const int _UNFINISHED = 0;
const int _RUNNING = 1;
const int _FINISHED = 2;

struct job_srtf : public job
{
    int _state;
    int remainingTime;
    job_srtf(const job& j);
};

class SRTF {
private:

    std::vector<job_srtf> _jobs;
    std::vector<job_srtf*> _schedulingList;

    int _totalTime;
    double _totalTime_with_weight;
    void schedulingInfo();
    void infoForPy();
    bool allFinished();

public:
    SRTF(const std::vector<job>& v);
    timeRecord run(bool);
    void outputSchedulingInfo();
};
auto srtfCompByRemaining = [](job_srtf* j1, job_srtf* j2)
{   return j1->remainingTime > j2->remainingTime; };

