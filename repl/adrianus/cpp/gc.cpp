#include "gc.h"

GarbageCollector::GarbageCollector() {
    //...
}

GarbageCollector::~GarbageCollector() {
    //...
}

void GarbageCollector::addEnvironment(Environment *env) {
    garbage_collect_environments.push_back(env);
}

void GarbageCollector::sweepEnvironments() {
    std::cout << "running sweepEnvironments()\n";
    for (Environment *env : garbage_collect_environments) {
        free_Ad_environment_memory(env);
    }
}

void GarbageCollector::clearAstNodes() {
    garbage_collect_ast_nodes.clear();
}

void GarbageCollector::clearEnvironments() {
    garbage_collect_environments.clear();
}

void GarbageCollector::clearObjects() {
    garbage_collect_objects.clear();
}