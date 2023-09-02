#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

#include <map>
#include "objects.h"
#include "gc.h"

class GarbageCollector; // forward declaration for GarbageCollector

class Environment {
public:
    std::map<std::string, Ad_Object*> store;
    Environment* outer;
    Environment* bootstrap;
    std::map<std::string, Environment*> siblings;
    int ref_count;
    bool isBootstrapEnvironment;
    bool isGlobalEnvironment;
    bool isRunningImportCommand;
    bool isInstanceEnvironment;
    Ad_Object* owningInstanceEnvironment;

    Environment();
    ~Environment();
    Ad_Object* Get(std::string);
    bool Check(std::string);
    void Set(std::string, Ad_Object*);
    void setLocalParam(std::string, Ad_Object*);
    void addSibling(std::string, Environment*);
    Environment* getSibling(std::string);
    void SetOuterEnvironment(Environment*);
    void SetBootstrapEnvironment(Environment*);
    Ad_Object* lookupOnlyInStore(std::string);
    Ad_Object* lookupConstructor();
    void FreeObjectForKey(std::string);
    void PrintStore(int);
    Ad_Object* storeToHashObject(GarbageCollector*);
    Ad_Object* contextToHashObject(GarbageCollector*);
    Environment* copy(GarbageCollector*);
};

Environment* newEnvironment();
Environment* newEnclosedEnvironment(Environment*);
Environment* newEnclosedEnvironmentUnfreeable(Environment*);

void free_Ad_environment_memory(Environment*);
void Ad_INCREF(Environment*);
void Ad_DECREF(Environment*);

#endif