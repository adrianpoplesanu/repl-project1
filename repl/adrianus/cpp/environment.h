#ifndef __ENVIRONMENT_H
#define __ENVIRONMENT_H

#include <map>
#include "objects.h"

class Environment {
private:
    std::map<std::string, Ad_Object*> store;
    std::map<std::string, Ad_Object*> outer; // aici e gresit, se construieste outer-ul iar alteratii ulterioare nu sunt luate in considerare, at trebui sa fie:
    //Environment* outer;
public:
    Environment();
    ~Environment();
    Ad_Object* Get(std::string);
    bool Check(std::string);
    void Set(std::string, Ad_Object*);
    void SetOuterEnvironment(Environment);
    void FreeObjectForKey(std::string);
    void PrintStore();
};

Environment NewEnvoronment();
Environment NewEnclosedEnvironment(Environment);

#endif