#pragma once

#include <cstddef>
#include <pthread.h>

bool start_pthread_with_stack(size_t stack_bytes, void* (*fn)(void*), void* arg, pthread_t* out);
