#include "objects.h"
#include <sstream>

AdCompiledFunction::AdCompiledFunction() {
    type = OBJ_COMPILED_FUNCTION;
    instructions = nullptr;
    num_locals = 0;
    num_parameters = 0;
}

std::string AdCompiledFunction::Inspect() {
    return "TODO: implement AdCompiledFunction.Inspect()";
}

void AdCompiledFunction::Print() {

}

Ad_Object_Type AdCompiledFunction::Type() {
    return OBJ_COMPILED_FUNCTION;
}

std::string AdCompiledFunction::Hash() {
    return "TODO: implement AdCompiledFunction.Hash()";
}

Ad_Object* AdCompiledFunction::copy(GarbageCollector* gc) {
    return NULL;
}

AdClosureObject::AdClosureObject() {
    type = OBJ_CLOSURE;
    fn = nullptr;
}

std::string AdClosureObject::Inspect() {
    return "todo: implement AdClosureObject.Inspect()";
}

void AdClosureObject::Print() {

}

Ad_Object_Type AdClosureObject::Type() {
    return OBJ_CLOSURE;
}

std::string AdClosureObject::Hash() {
    return "todo: implement AdClosureObject.Hash()";
}

Ad_Object* AdClosureObject::copy(GarbageCollector*) {
    return NULL;
}

AdCompiledClass::AdCompiledClass() {
    type = OBJ_COMPILED_CLASS;
    ref_count = 0;
    marked = false;
}

std::string AdCompiledClass::Inspect() {
    std::stringstream ss;
    ss << std::hex << this;
    return "<class object at memory address: " + ss.str() + ">";
}

std::string AdCompiledClass::repr() {
    return Inspect();
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
    type = OBJ_COMPILED_INSTANCE;
    ref_count = 0;
    marked = false;
    klass = nullptr;
    definition_num_args = 0;
}

std::string AdCompiledInstance::Inspect() {
    std::stringstream ss;
    ss << std::hex << this;
    return "<class instance at memory address: " + ss.str() + ">";
}

std::string AdCompiledInstance::repr() {
    return Inspect();
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
    type = OBJ_BOUND_METHOD;
    ref_count = 0;
    marked = false;
    owner = nullptr;
    bound_method = nullptr;
}

AdBoundMethod::AdBoundMethod(AdCompiledInstance* o, AdClosureObject* bm) {
    type = OBJ_BOUND_METHOD;
    ref_count = 0;
    marked = false;
    owner = o;
    bound_method = bm;
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

AdRuntimeBoundMethod::AdRuntimeBoundMethod() {
    type = OBJ_RUNTIME_BOUND_METHOD;
    ref_count = 0;
    marked = false;
    receiver = nullptr;
    method_name = "";
}

AdRuntimeBoundMethod::AdRuntimeBoundMethod(Ad_Object* recv, std::string method) {
    type = OBJ_RUNTIME_BOUND_METHOD;
    ref_count = 0;
    marked = false;
    receiver = recv;
    method_name = std::move(method);
}

std::string AdRuntimeBoundMethod::Inspect() {
    return "todo: implement AdRuntimeBoundMethod.Inspect()";
}

void AdRuntimeBoundMethod::Print() {
}

Ad_Object_Type AdRuntimeBoundMethod::Type() {
    return type;
}

std::string AdRuntimeBoundMethod::Hash() {
    return "todo: implement AdRuntimeBoundMethod.Hash()";
}

Ad_Object* AdRuntimeBoundMethod::copy(GarbageCollector*) {
    return NULL;
}
