#include "profiling.h"

ExecutionTimeProfiling::ExecutionTimeProfiling(std::string executionTimeProfilingName) {
    name = executionTimeProfilingName;
    begin = 0;
    end = 0;
}

void ExecutionTimeProfiling::start() {
    begin = clock();
}

void ExecutionTimeProfiling::stop() {
    end = clock();
}

void ExecutionTimeProfiling::showMetric() {
    std::cout << "[ " << name << " ] ran for: " << double(end - begin) / CLOCKS_PER_SEC << "secs\n";
    for (const auto& entry : poi) {
        std::cout << "[ " << name << " ] [ " << entry.first << " ] ran for: " << double(entry.second) / CLOCKS_PER_SEC << "secs\n";
    }
}

void ExecutionTimeProfiling::start(const std::string& metricName) {
    if (poi.find(metricName) != poi.end()) {
        poi_count[metricName]++;
        if (poi_count[metricName] > 1) {
            std::cout << "err! check profiled code region surroundings\n";
        }
        poi_begin[metricName] = clock();
    } else {
        poi[metricName] = 0.0;
        poi_count[metricName] = 1;
        poi_begin[metricName] = clock();
    }
}

void ExecutionTimeProfiling::stop(const std::string& metricName) {
    if (poi.find(metricName) != poi.end()) {
        poi_count[metricName]--;
        if (poi_count[metricName] == 0) {
            poi_end[metricName] = clock();
            poi[metricName] += double(poi_end[metricName] - poi_begin[metricName]); // / CLOCKS_PER_SEC;
            //std::cout << "[ " << name << " ] [ " << metricName << " ] ran for: " << double(poi_end[metricName] - poi_begin[metricName]) / CLOCKS_PER_SEC << "secs\n";
        }
    } else {
        //std::cout << metricName << " terminated";
    }
}

MemoryProfiling::MemoryProfiling() {
    totalAllocated = 0;
    totalFreed = 0;
    numAllocations = 0;
    numFreed = 0;
}

void MemoryProfiling::addAllocated(int size) {
    totalAllocated += size;
}

void MemoryProfiling::addFreed(int size) {
    totalFreed += size;
}

void MemoryProfiling::incrementAllocation() {
    numAllocations++;
}

void MemoryProfiling::incrementFreed() {
    numFreed++;
}

ExecutionTimeProfiling executionTimeProfiling("ad");
