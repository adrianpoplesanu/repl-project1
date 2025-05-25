#include "objects.h"

AdCompiledFunction::AdCompiledFunction() {
    //...
}

std::string AdCompiledFunction::Inspect() {
    return "TODO: implement AdCompiledFunction.Inspect()";
}

void AdCompiledFunction::Print() {

}

Ad_Object_Type AdCompiledFunction::Type() {
    return OBJ_NULL;
}

std::string AdCompiledFunction::Hash() {
    return "TODO: implement AdCompiledFunction.Hash()";
}

Ad_Object* AdCompiledFunction::copy(GarbageCollector* gc) {
    return NULL;
}