#include "thread_workers.h"
#include "objects.h"
#include "evaluator.h"
#include "environment.h"
#include "vm/vm.h"
#include "vm/vm_context.h"

int TOTAL_THREADS_RUNNING = 0;
std::vector<Ad_Object*> threadPool;

namespace {

Ad_Object* invoke_thread_callback(Ad_Object* rawCallback, std::vector<Ad_Object*> params, GarbageCollector* gc, Environment* parent_env) {
    if (rawCallback == nullptr) {
        return &NULLOBJECT;
    }
    if (rawCallback->type == OBJ_CLOSURE || rawCallback->type == OBJ_BOUND_METHOD) {
        VM vm;
        vm.gc = gc;
        if (VM* parent = ad_current_vm()) {
            vm.constants = parent->constants;
            vm.globals = parent->globals;
            vm.global_names = parent->global_names;
            vm.bootstrap_global_names = parent->bootstrap_global_names;
        }
        return vm.invoke_callable(rawCallback, params);
    }
    if (rawCallback->type == OBJ_FUNCTION) {
        Evaluator evaluator;
        evaluator.setGarbageCollector(gc);
        std::unordered_map<std::string, Ad_Object*> kw_args;
        // Prefer the defining closure env; fall back to the parent program env.
        Environment scratch;
        Environment* call_env = parent_env;
        if (call_env == nullptr) {
            Ad_Function_Object* func = (Ad_Function_Object*) rawCallback;
            call_env = func->env != nullptr ? func->env : &scratch;
        }
        return evaluator.ApplyFunction(rawCallback, params, kw_args, *call_env);
    }
    return &NULLOBJECT;
}

} // namespace

void ad_worker_async(Ad_Object* rawCallback, std::vector<Ad_Object*> params, Ad_Object* rawObject, GarbageCollector *gc, Environment* env) {
    Ad_Object* result = invoke_thread_callback(rawCallback, params, gc, env);
    Ad_Thread_Object *threadObject = (Ad_Thread_Object*) rawObject;
    threadObject->result = result;
    TOTAL_THREADS_RUNNING--;
}

void ad_worker_blocking(Ad_Object* rawCallback, std::vector<Ad_Object*> params, Ad_Object* rawObject, GarbageCollector *gc, Environment* env) {
    Ad_Object* result = invoke_thread_callback(rawCallback, params, gc, env);
    Ad_Thread_Object *threadObject = (Ad_Thread_Object*) rawObject;
    threadObject->result = result;
    TOTAL_THREADS_RUNNING--;
}