#ifndef __EVAL_UTILS_H
#define __EVAL_UTILS_H

#include "repl.h"
#include "parser.h"
#include "evaluator.h"
#include "environment.h"
#include "gc.h"

void evalSource(std::string, Environment*, GarbageCollector*);
void importSource(std::string, Environment*, GarbageCollector*);
void unescapeSource(std::string&);
void importSource(std::string);

Ad_Object* dispatchMemberAccessPerObjectType(Ad_Object*, Ad_AST_Identifier*, std::vector<Ad_Object*>, GarbageCollector*);

#endif