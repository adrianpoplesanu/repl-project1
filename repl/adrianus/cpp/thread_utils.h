#ifndef __THREAD_UTILS_H
#define __THREAD_UTILS_H

void thread_callback(Ad_Object*, std::vector<Ad_Object*>);
void thread_async_run(Ad_Object*, GarbageCollector*, Environment&);
void thread_blocking_run(Ad_Object*, GarbageCollector*, Environment&);

#endif