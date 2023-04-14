#include "thread_utils.h"
#include "thread_workers.h"
#include "objects.h"
#include "gc.h"
#include <chrono>
#include <thread>
#include <iostream>
#include "gc.h"
#include "environment.h"

void thread_callback(Ad_Object* rawObject, std::vector<Ad_Object*> arg_objs) {
    Ad_Thread_Object *threadObject = (Ad_Thread_Object*) rawObject;
    threadObject->callback = arg_objs.at(0);
}

void thread_async_run(Ad_Object* rawObject, GarbageCollector *gc, Environment &env) {
    Ad_Thread_Object* threadObject = (Ad_Thread_Object*) rawObject;
    TOTAL_THREADS_RUNNING++;
    std::thread th1(ad_worker_async, threadObject->callback, gc, &env);
    th1.detach();
}

void thread_blocking_run(Ad_Object* rawObject, GarbageCollector *gc, Environment &env) {
    Ad_Thread_Object* threadObject = (Ad_Thread_Object*) rawObject;
    std::thread th1(ad_worker_blocking, threadObject->callback, gc, &env);
    th1.join();
}