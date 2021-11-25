#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

#include <map>
#include "objects.h"

class Environment {
public:
    std::map<std::string, Ad_Object*> store;
    Environment* outer;

    Environment();
    ~Environment();
    Ad_Object* Get(std::string);
    bool Check(std::string);
    void Set(std::string, Ad_Object*);
    void SetCallArgument(std::string, Ad_Object*);
    void SetOuterEnvironment(Environment*);
    void FreeObjectForKey(std::string);
    void PrintStore();
};

Environment NewEnvironment();
Environment NewEnclosedEnvironment(Environment*);

#endif