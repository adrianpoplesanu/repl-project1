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

AdClosureObject::AdClosureObject() {
    //...
}

std::string AdClosureObject::Inspect() {
    return "todo: implement AdClosureObject.Inspect()";
}

void AdClosureObject::Print() {

}

Ad_Object_Type AdClosureObject::Type() {
    return type;
}

std::string AdClosureObject::Hash() {
    return "todo: implement AdClosureObject.Hash()";
}

Ad_Object* AdClosureObject::copy(GarbageCollector*) {
    return NULL;
}

AdCompiledClass::AdCompiledClass() {
    //...
}

std::string AdCompiledClass::Inspect() {
    return "todo: implement AdCompiledClass.Inspect()";
}

void AdCompiledClass::Print() {

}

Ad_Object_Type AdCompiledClass::Type() {
    return type;
}

std::string AdCompiledClass::Hash() {
    return "todo: implement AdCompiledClass.Hash()";
}

Ad_Object* AdCompiledClass::copy(GarbageCollector*) {
    return NULL;
}


AdCompiledInstance::AdCompiledInstance() {
    //...
}

std::string AdCompiledInstance::Inspect() {
    return "todo: implement AdCompiledInstance.Inspect()";
}

void AdCompiledInstance::Print() {

}

Ad_Object_Type AdCompiledInstance::Type() {
    return type;
}

std::string AdCompiledInstance::Hash() {
    return "todo: implement AdCompiledInstance.Hash()";
}

Ad_Object* AdCompiledInstance::copy(GarbageCollector*) {
    return NULL;
}

AdBoundMethod::AdBoundMethod() {
    //...
}

std::string AdBoundMethod::Inspect() {
    return "todo: implement AdBoundMethod.Inspect()";
}

void AdBoundMethod::Print() {

}

Ad_Object_Type AdBoundMethod::Type() {
    return type;
}

std::string AdBoundMethod::Hash() {
    return "todo: implement AdBoundMethod.Hash()";
}

Ad_Object* AdBoundMethod::copy(GarbageCollector*) {
    return NULL;
}
