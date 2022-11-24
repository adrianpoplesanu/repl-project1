#include "gc.h"

GarbageCollector::GarbageCollector() {
    //...
}

GarbageCollector::~GarbageCollector() {
    //...
}

void GarbageCollector::addEnvironment(Environment *env) {
    gc_environments.push_back(env);
}

void GarbageCollector::sweepEnvironments() {
    std::cout << "running sweepEnvironments()\n";
    int count = 0;
    std::vector<Environment*> referencedEnvironments;
    for (Environment *env : gc_environments) {
        if (env->ref_count > 0) {
            //...
            referencedEnvironments.push_back(env);
        } else {
            count++;
            free_Ad_environment_memory(env);
        }
    }
    std::cout << "freed " << count << " environments\n";
    gc_environments = referencedEnvironments;
}

void GarbageCollector::forceFreeEnvironments() {
    for (Environment *env : gc_environments) {
        env->ref_count = 0;
        free_Ad_environment_memory(env);
    }
}

void GarbageCollector::clearAstNodes() {
    gc_ast_nodes.clear();
}

void GarbageCollector::clearEnvironments() {
    gc_environments.clear();
}

void GarbageCollector::clearObjects() {
    gc_objects.clear();
}