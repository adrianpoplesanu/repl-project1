#include "stack_thread.h"

#include <cerrno>
#include <cstring>

bool start_pthread_with_stack(size_t stack_bytes, void* (*fn)(void*), void* arg, pthread_t* out) {
    pthread_attr_t attr;
    if (pthread_attr_init(&attr) != 0) {
        return false;
    }
    if (pthread_attr_setstacksize(&attr, stack_bytes) != 0) {
        pthread_attr_destroy(&attr);
        return false;
    }
    const int rc = pthread_create(out, &attr, fn, arg);
    pthread_attr_destroy(&attr);
    return rc == 0;
}
