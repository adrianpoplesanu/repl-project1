#include "thread_utils.h"
#include "thread_workers.h"
#include "objects.h"
#include "gc.h"
#include <chrono>
#include <thread>
#include <iostream>
#include "gc.h"
#include "environment.h"

void test() {
    std::cout << "start test thread\n";
    for (int i = 0; i < 100; i++) {
        std::cout << i << "\n";
    }
    std::cout << "finish test thread\n";
    TOTAL_THREADS_RUNNING--;
}

void thread_callback(Ad_Object* rawObject, std::vector<Ad_Object*> arg_objs) {
    Ad_Thread_Object *threadObject = (Ad_Thread_Object*) rawObject;

    GarbageCollector *gc2 = new GarbageCollector();
    threadObject->internal_gc = gc2;

    threadObject->callback = arg_objs.at(0);
    std::vector<Ad_Object*> params;
    for (int i = 1; i < arg_objs.size(); i++) {
        params.push_back(arg_objs.at(i)->copy(threadObject->internal_gc));
    }
    threadObject->params = params;
}

void thread_async_run(Ad_Object* rawObject, GarbageCollector *gc, Environment &env) {
    Ad_Thread_Object* threadObject = (Ad_Thread_Object*) rawObject;
    TOTAL_THREADS_RUNNING++;
    std::thread *th1 = new std::thread(ad_worker_async, threadObject->callback, threadObject->params, threadObject->internal_gc, &env);
    threadObject->internal_thread = th1;
    threadPool.push_back(threadObject);
}

void thread_blocking_run(Ad_Object* rawObject, GarbageCollector *gc, Environment &env) {
    Ad_Thread_Object* threadObject = (Ad_Thread_Object*) rawObject;
    // TODO: better structure for this
    TOTAL_THREADS_RUNNING++;
    std::thread *th1 = new std::thread(ad_worker_blocking, threadObject->callback, threadObject->params, threadObject->internal_gc, &env);
    threadObject->internal_thread = th1;
    threadPool.push_back(threadObject);
    th1->join();
}

void sleep_builtin_executor(int value) {
    std::this_thread::sleep_for (std::chrono::milliseconds(value));
}