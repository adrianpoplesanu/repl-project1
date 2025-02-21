#ifndef AD_PROFILING_H
#define AD_PROFILING_H

#include <iostream>
#include <memory>

class ExecutionTimeProfiling {
private:
    clock_t begin;
    clock_t end;
    std::string name;

    std::unordered_map<std::string, double> poi;
    std::unordered_map<std::string, int> poi_count;
    std::unordered_map<std::string, clock_t> poi_begin;
    std::unordered_map<std::string, clock_t> poi_end;
public:
    explicit ExecutionTimeProfiling(std::string);
    void start();
    void stop();
    void showMetric();

    void start(const std::string&);
    void stop(const std::string&);
};

class MemoryProfiling {
private:
    int totalAllocated;
    int totalFreed;
    int numAllocations;
    int numFreed;

public:
    std::string name;

    MemoryProfiling(std::string);
    void addAllocated(int);
    void addFreed(int);
    void printUsage();
    void incrementAllocation();
    void incrementFreed();
    void showTotalResidualGCObjects(GarbageCollector*);
};

#endif //AD_PROFILING_H
