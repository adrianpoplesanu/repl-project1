#include "profiling.h"

ExecutionTimeProfiling::ExecutionTimeProfiling(std::string n) {
    name = n;
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
}
