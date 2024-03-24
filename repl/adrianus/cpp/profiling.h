#ifndef AD_PROFILING_H
#define AD_PROFILING_H

#include <iostream>
#include <memory>

class ExecutionTimeProfiling {
private:
    clock_t begin;
    clock_t end;
    std::string name;
public:
    explicit ExecutionTimeProfiling(std::string);
    void start();
    void stop();
    void showMetric();
};

#endif //AD_PROFILING_H
