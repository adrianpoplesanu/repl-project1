#ifndef __ENVIRONMENT_BUILTINS_H
#define __ENVIRONMENT_BUILTINS_H

#include <map>
#include "objects.h"

class EnvironmentBuiltins {
public:
    Ad_Object* get(std::string);
    void set(std::string, Ad_Object*);
};

#endif