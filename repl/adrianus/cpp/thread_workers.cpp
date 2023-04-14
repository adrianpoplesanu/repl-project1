#include "thread_workers.h"
#include "objects.h"
#include "evaluator.h"
#include "environment.h"

void ad_worker_async(Ad_Object* rawCallback, GarbageCollector *gc, Environment* env) {
    if (rawCallback->type == OBJ_FUNCTION) {
        Evaluator evaluator;
        evaluator.setGarbageCollector(gc);
        std::vector<Ad_Object*> arg_objs;
        evaluator.ApplyFunction(rawCallback, arg_objs, *env);
        TOTAL_THREADS_RUNNING--;
    }
}

void ad_worker_blocking(Ad_Object* rawCallback, GarbageCollector *gc, Environment* env) {
    if (rawCallback->type == OBJ_FUNCTION) {
        Evaluator evaluator;
        evaluator.setGarbageCollector(gc);
        std::vector<Ad_Object*> arg_objs;
        evaluator.ApplyFunction(rawCallback, arg_objs, *env);
    }
}