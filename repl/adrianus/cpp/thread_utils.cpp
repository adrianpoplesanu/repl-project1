#include "thread_utils.h"
#include "objects.h"
#include <chrono>
#include <thread>
#include <iostream>

void test_worker() {
    // temp for testing raw functionality
    std::cout << "starting thread\n";
    for (int i = 0; i < 100; i++) {
        std::cout << i << "\n";
    }
    std::cout << "finishing thread\n";
}

void thread_callback(Ad_Object* rawObject, std::vector<Ad_Object*> arg_objs) {
    Ad_Thread_Object *threadObject = (Ad_Thread_Object*) rawObject;
    threadObject->callback = arg_objs.at(0);
}

void thread_async_run(Ad_Object* rawObject) {
    std::thread th1(test_worker);
    th1.detach();
}

void thread_blocking_run(Ad_Object* rawObject) {
    std::thread th1(test_worker);
    th1.join();
}