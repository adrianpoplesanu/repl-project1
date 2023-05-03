#ifndef __THREAD_WORKERS_H
#define __THREAD_WORKERS_H

int TOTAL_THREADS_RUNNING = 0;
std::vector<Ad_Object*> threadPool;
void ad_worker_async(Ad_Object*, std::vector<Ad_Object*>, GarbageCollector*, Environment*);
void ad_worker_blocking(Ad_Object*, GarbageCollector*, Environment*);

#endif