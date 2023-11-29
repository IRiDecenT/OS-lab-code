#pragma once
#include "utils.h"

class job
{
private:
    std::string _jobName;
    int _arrivalTime;
    int _runTime;
    int _priority;
    // 为了画图，保存其运行时间片的数据
    std::vector<timePeriod> _runPeriod;


public:
    job();
    job(const std::string& name, int arrivaltime = 0, int runtime = 0, int priority = 0);
    friend std::ostream& operator<<(std::ostream&, const job&);
    int arrivalTime()  const  { return _arrivalTime;  }
    int runTime() const   { return _runTime;   }
    int priority() const  { return _priority;  }
    std::string name() const { return _jobName; }
    std::vector<timePeriod>& getRunPeriod() { return _runPeriod; }
    const std::vector<timePeriod>& getRunPeriod() const  { return _runPeriod; }
};

struct compByArrivalFirst
{
    bool operator()(const job& j1, const job& j2) { return j1.arrivalTime() < j2.arrivalTime(); }
};

auto JobComp = [](const job& j1, const job& j2){
        return j1.arrivalTime() < j2.arrivalTime();
    };

void printTime(const timeRecord& t);
