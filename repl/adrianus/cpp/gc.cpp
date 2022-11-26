#include "gc.h"

GarbageCollector::GarbageCollector() {
    //...
}

GarbageCollector::~GarbageCollector() {
    //std::cout << "when quitting garbage collector i still have " << gc_environments.size() << " environments\n";
    forceFreeEnvironments();
}

void GarbageCollector::addEnvironment(Environment *env) {
    //std::cout << "added new environment to gc\n";
    gc_environments.push_back(env);
}

void GarbageCollector::sweepEnvironments() {
    //std::cout << "running sweepEnvironments()\n";
    int count = 0;
    std::vector<Environment*> referencedEnvironments;
    //std::cout << "trying to delete " << gc_environments.size() << " environments\n";
    for (Environment *env : gc_environments) {
        if (env->ref_count > 0) {
            //...
            referencedEnvironments.push_back(env);
        } else {
            count++;
            free_Ad_environment_memory(env);
        }
    }
    //std::cout << "freed " << count << " environments\n";
    //std::cout << "skipped " << referencedEnvironments.size() << " environments\n";
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

void GarbageCollector::scheduleEnvironmentToDECREF(Environment *env) {
    scheduled_to_DECREF_environments.push_back(env);
}

void GarbageCollector::consumeScheduledDECREFEnvironments() {
    for (Environment *env : scheduled_to_DECREF_environments) {
        Ad_DECREF(env);
    }
    scheduled_to_DECREF_environments.clear();
}