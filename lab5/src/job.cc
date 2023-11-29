#include "job.h"

job::job():
    _jobName(std::string()),
    _arrivalTime(0),
    _runTime(0),
    _priority(0){}

job::job(const std::string& name,
        int arrivaltime,
        int runtime,
        int priority)
    :_jobName(name),
    _arrivalTime(arrivaltime),
    _runTime(runtime),
    _priority(priority){}

std::ostream &operator<<(std::ostream & out, const job & job)
{
    out<< "job name: " << job._jobName
        <<"\t arrival time: " << job._arrivalTime
        <<"\t run time: " << job._runTime
        <<"\t priority: " << job._priority;
    out<<std::endl;
    return out;
}

void printTime(const timeRecord& t)
{
    std::cout<<"平均周转时间:" << " " << t.first << std::endl;
    std::cout<<"平均带权周转时间:" << " " << t.second << std::endl << std::endl;
}
