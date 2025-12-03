#ifndef __THREAD_WORKERS_H
#define __THREAD_WORKERS_H

#include <vector>
class Ad_Object;
class GarbageCollector;
class Environment;

extern int TOTAL_THREADS_RUNNING;
extern std::vector<Ad_Object*> threadPool;
void ad_worker_async(Ad_Object*, std::vector<Ad_Object*>, Ad_Object*, GarbageCollector*, Environment*);
void ad_worker_blocking(Ad_Object*, std::vector<Ad_Object*>, Ad_Object*, GarbageCollector*, Environment*);

#endif