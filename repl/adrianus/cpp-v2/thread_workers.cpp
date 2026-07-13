#include "thread_workers.h"
#include "objects.h"
#include "evaluator.h"
#include "environment.h"
#include "vm/vm.h"
#include "vm/vm_context.h"

int TOTAL_THREADS_RUNNING = 0;
std::vector<Ad_Object*> threadPool;

namespace {

Ad_Object* invoke_thread_callback(Ad_Object* rawCallback, std::vector<Ad_Object*> params, GarbageCollector* gc) {
    if (rawCallback->type == OBJ_CLOSURE) {
        VM vm;
        vm.gc = gc;
        if (VM* parent = ad_current_vm()) {
            vm.constants = parent->constants;
            vm.globals = parent->globals;
            vm.global_names = parent->global_names;
            vm.bootstrap_global_names = parent->bootstrap_global_names;
        }
        return vm.invoke_closure(static_cast<AdClosureObject*>(rawCallback), params);
    }
    if (rawCallback->type == OBJ_FUNCTION) {
        Evaluator evaluator;
        evaluator.setGarbageCollector(gc);
        std::unordered_map<std::string, Ad_Object*> kw_args;
        Environment env;
        return evaluator.ApplyFunction(rawCallback, params, kw_args, env);
    }
    return &NULLOBJECT;
}

} // namespace

void ad_worker_async(Ad_Object* rawCallback, std::vector<Ad_Object*> params, Ad_Object* rawObject, GarbageCollector *gc, Environment* env) {
    (void)env;
    Ad_Object* result = invoke_thread_callback(rawCallback, params, gc);
    Ad_Thread_Object *threadObject = (Ad_Thread_Object*) rawObject;
    threadObject->result = result;
    TOTAL_THREADS_RUNNING--;
}

void ad_worker_blocking(Ad_Object* rawCallback, std::vector<Ad_Object*> params, Ad_Object* rawObject, GarbageCollector *gc, Environment* env) {
    (void)env;
    Ad_Object* result = invoke_thread_callback(rawCallback, params, gc);
    Ad_Thread_Object *threadObject = (Ad_Thread_Object*) rawObject;
    threadObject->result = result;
    TOTAL_THREADS_RUNNING--;
}