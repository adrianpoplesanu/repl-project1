#ifndef __EVAL_UTILS_H
#define __EVAL_UTILS_H

#include "repl.h"
#include "parser.h"
#include "evaluator.h"
#include "environment.h"
#include "gc.h"

void evalSource(std::string, Environment*, GarbageCollector*);

#endif