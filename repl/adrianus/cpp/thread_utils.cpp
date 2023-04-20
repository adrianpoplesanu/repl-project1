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
    threadObject->callback = arg_objs.at(0);
}

void thread_async_run(Ad_Object* rawObject, GarbageCollector *gc, Environment &env) {
    Ad_Thread_Object* threadObject = (Ad_Thread_Object*) rawObject;
    TOTAL_THREADS_RUNNING++;
    //std::thread th1(ad_worker_async, threadObject->callback, gc, &env);
    //std::thread th1(test);
    //th1.detach();
    //std::thread* th1 = new std::thread(test);
    //std::thread *th1 = new std::thread(ad_worker_async, threadObject->callback->copy(gc), gc, (&env)->copy(gc)); // TODO: asta nu merge
    //std::cout << "pe aici\n";
    //std::thread *th1 = new std::thread(ad_worker_async, ((Ad_Function_Object*)threadObject->callback)->copy(gc), gc, &env);
    //std::thread *th1 = new std::thread(ad_worker_async, threadObject->callback, gc, &env);

    //Environment *env2 = newEnvironment();
    GarbageCollector *gc2 = new GarbageCollector();
    std::thread *th1 = new std::thread(ad_worker_async, threadObject->callback, gc2, &env);
    threadObject->internal_thread = th1;
    threadObject->internal_gc = gc2;
    threadPool.push_back(threadObject);
}

void thread_blocking_run(Ad_Object* rawObject, GarbageCollector *gc, Environment &env) {
    Ad_Thread_Object* threadObject = (Ad_Thread_Object*) rawObject;
    std::thread th1(ad_worker_blocking, threadObject->callback, gc, &env);
    th1.join();
}