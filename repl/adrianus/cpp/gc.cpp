#include "gc.h"

GarbageCollector::GarbageCollector() {
    //...
}

GarbageCollector::~GarbageCollector() {
    //...
}

void GarbageCollector::markEnvironment(Environment *env) {
    garbage_collect_environments.push_back(env);
}

void GarbageCollector::sweepEnvironments() {
    for (Environment *env : garbage_collect_environments) {
        std::cout << env << "\n";
    }
}