#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

#include <map>
#include "objects.h"

class Environment {
public:
    std::map<std::string, Ad_Object*> store;
    Environment* outer;
    Environment* bootstrap;
    int ref_count;
    bool isBootstrapEnvironment;
    bool isGlobalEnvironment;

    Environment();
    ~Environment();
    Ad_Object* Get(std::string);
    bool Check(std::string);
    void Set(std::string, Ad_Object*);
    void SetCallArgument(std::string, Ad_Object*);
    void SetOuterEnvironment(Environment*);
    void SetBootstrapEnvironment(Environment*);
    void FreeObjectForKey(std::string);
    void PrintStore(int);
};

Environment NewEnvironment();
Environment NewEnclosedEnvironment(Environment*);
Environment* newEnclosedEnvironment(Environment*);
Environment NewEnclosedEnvironment(Environment*, Environment*);
Environment* newEnclosedEnvironmentUnfreeable(Environment*);

void free_Ad_environment_memory(Environment*);
void Ad_INCREF(Environment*);
void Ad_DECREF(Environment*);

#endif