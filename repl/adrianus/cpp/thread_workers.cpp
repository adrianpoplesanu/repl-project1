#include "thread_workers.h"
#include "objects.h"
#include "evaluator.h"
#include "environment.h"

void ad_worker_async(Ad_Object* rawCallback, std::vector<Ad_Object*> params, GarbageCollector *gc, Environment* env) {
    //std::cout << "ad_worker_async()\n";
    if (rawCallback->type == OBJ_FUNCTION) {
        //std::cout << "evaluating function\n";
        //std::cout << rawCallback->Inspect() << "\n";
        Evaluator evaluator;
        evaluator.setGarbageCollector(gc);
        //std::vector<Ad_Object*> arg_objs;
        //std::cout << "number of params: " << params.size() << "\n";
        evaluator.ApplyFunction(rawCallback, params, *env);
        TOTAL_THREADS_RUNNING--;
    }
}

void ad_worker_blocking(Ad_Object* rawCallback, std::vector<Ad_Object*> params, GarbageCollector *gc, Environment* env) {
    if (rawCallback->type == OBJ_FUNCTION) {
        Evaluator evaluator;
        evaluator.setGarbageCollector(gc);
        //std::vector<Ad_Object*> arg_objs;
        evaluator.ApplyFunction(rawCallback, params, *env);
        TOTAL_THREADS_RUNNING--;
    }
}