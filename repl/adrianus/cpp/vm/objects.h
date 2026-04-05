#ifndef AD_VM_OBJECTS_H
#define AD_VM_OBJECTS_H

#include <string>
#include <unordered_map>
#include <vector>
#include "../objects.h"
#include "../gc.h"
#include "instructions.h"

class AdCompiledFunction : public Ad_Object {
public:
    Instructions* instructions;
    int num_locals;
    int num_parameters;

    AdCompiledFunction();
    std::string Inspect() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

class AdClosureObject : public Ad_Object {
public:
    AdCompiledFunction* fn;

	AdClosureObject();
    std::string Inspect() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

class AdCompiledClass : public Ad_Object {
public:
    std::unordered_map<std::string, AdClosureObject*> methods;
    std::vector<AdCompiledFunction*> field_initializers;

    AdCompiledClass();
    std::string Inspect() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

class AdCompiledInstance : public Ad_Object {
public:
    AdCompiledClass* klass;
    int definition_num_args;

    AdCompiledInstance();
    std::string Inspect() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

class AdBoundMethod : public Ad_Object {
public:
    AdCompiledInstance* owner;
    AdClosureObject* bound_method;

    AdBoundMethod();
    AdBoundMethod(AdCompiledInstance* owner, AdClosureObject* bound_method);
    std::string Inspect() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

#endif