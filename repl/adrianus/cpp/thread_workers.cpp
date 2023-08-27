#include "thread_workers.h"
#include "objects.h"
#include "evaluator.h"
#include "environment.h"

void ad_worker_async(Ad_Object* rawCallback, std::vector<Ad_Object*> params, Ad_Object* rawObject, GarbageCollector *gc, Environment* env) {
    if (rawCallback->type == OBJ_FUNCTION) {
        Evaluator evaluator;
        evaluator.setGarbageCollector(gc);
        Ad_Object* result = evaluator.ApplyFunction(rawCallback, params, *env);
        Ad_Thread_Object *threadObject = (Ad_Thread_Object*) rawObject;
        threadObject->result = result;
        TOTAL_THREADS_RUNNING--;
    }
}

void ad_worker_blocking(Ad_Object* rawCallback, std::vector<Ad_Object*> params, Ad_Object* rawObject, GarbageCollector *gc, Environment* env) {
    if (rawCallback->type == OBJ_FUNCTION) {
        Evaluator evaluator;
        evaluator.setGarbageCollector(gc);
        Ad_Object* result = evaluator.ApplyFunction(rawCallback, params, *env);
        Ad_Thread_Object *threadObject = (Ad_Thread_Object*) rawObject;
        threadObject->result = result;
        TOTAL_THREADS_RUNNING--;
    }
}