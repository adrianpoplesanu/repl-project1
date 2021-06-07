#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

#include <map>
#include "objects.h"

class Environment {
private:
    std::map<std::string, Ad_Object*> store;
public:
    Environment();
    ~Environment();
    Ad_Object* Get(std::string);
    void Set(std::string, Ad_Object*);
    void PrintStore();
};

Environment NewEnvoronment();
Environment NewEnclosedEnvironment(Environment);

#endif