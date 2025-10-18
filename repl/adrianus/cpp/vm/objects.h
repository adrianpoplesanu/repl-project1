#ifndef AD_VM_OBJECTS_H
#define AD_VM_OBJECTS_H

#include <string>
#include "../objects.h"
#include "../gc.h"

class AdCompiledFunction : public Ad_Object {
public:
    AdCompiledFunction();
    std::string Inspect() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

class AdClosureObject : public Ad_Object {
public:
	AdClosureObject();
    std::string Inspect() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

class AdCompiledClass : public Ad_Object {
public:
    AdCompiledClass();
    std::string Inspect() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

class AdCompiledInstance : public Ad_Object {
public:
    AdCompiledInstance();
    std::string Inspect() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

class AdBoundMethod : public Ad_Object {
public:
    AdBoundMethod();
    std::string Inspect() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

#endif