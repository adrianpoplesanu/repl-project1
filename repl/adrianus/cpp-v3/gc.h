#ifndef __GC_H
#define __GC_H

#include <vector>

class Environment;
class Ad_Object;
class Ad_AST_Node;
class VM;

class GarbageCollector {
public:
    std::vector<Environment*> gc_environments;
    std::vector<Environment*> scheduled_to_DECREF_environments;
    std::vector<Ad_Object*> gc_objects;
    std::vector<Ad_AST_Node*> gc_ast_nodes;

    Environment* mainEnv;
    Ad_Object* head;
    Ad_Object* tail;

    int cycle1;
    int cycle2;
    int cycle3;
    int cycleVM;
    int maxCycle1 = 100;
    int maxCycle2 = 100;
    int maxCycle3 = 100;
    /// VM instructions between mid-run mark/sweep collections.
    int maxCycleVM = 10000;
    /// When false, VM skips periodic mark/sweep (end-of-run forceFree still runs).
    bool vmMidRunCollection = true;
    //int maxCycle1 = 0;
    //int maxCycle2 = 0;
    //int maxCycle3 = 0;

    GarbageCollector();
    ~GarbageCollector();
    void addEnvironment(Environment*);
    void addObject(Ad_Object*);
    void addAstNode(Ad_AST_Node*);
    void clearEnvironments();
    void clearObjects();
    void clearAstNodes();
    void sweepEnvironments();
    void markObjects();
    void markObject(Ad_Object*);
    void markObjects(Ad_Object *stack[2048], int sp);
    void markObjectsVM(VM*);
    void unmarkAllObjects();
    void sweepObjects();
    void sweepObjectsVM();
    void sweepAstNodes();
    void forceFreeEnvironments();
    void forceFreeObjects();
    void forceFreeAstNodes();
    void scheduleEnvironmentToDECREF(Environment*);
    void consumeScheduledDECREFEnvironments();
    int getTotalResidualGCObjects();
};

#endif