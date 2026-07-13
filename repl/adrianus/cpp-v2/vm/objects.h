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
    /// Local slot index → name (for `__locals()` inside functions).
    std::vector<std::pair<std::string, int>> local_names;
    /// Default argument values for trailing parameters (evaluator parity).
    std::vector<Ad_Object*> default_arg_values;

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
    /// Captured locals/globals from enclosing scopes (VM parallels evaluator free bindings).
    /// Index matches `SymbolScope::FREE` symbol index emitted by `OP_GET_FREE`.
    std::vector<Ad_Object*> free_vars;

	AdClosureObject();
    std::string Inspect() override;
    std::string repr() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

class AdCompiledClass : public Ad_Object {
public:
    std::string name;
    std::unordered_map<std::string, AdClosureObject*> methods;
    std::vector<AdCompiledFunction*> field_initializers;
    /// Maps field name → slot index in `AdCompiledInstance::fields`.
    std::unordered_map<std::string, int> field_name_to_index;
    /// Resolved parent classes in declaration order.
    std::vector<AdCompiledClass*> supers;
    /// Parent lookup for `super(Parent).method(...)`.
    std::unordered_map<std::string, AdCompiledClass*> super_classes_by_name;

    AdCompiledClass();
    std::string Inspect() override;
    std::string repr() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

class AdCompiledInstance : public Ad_Object {
public:
    AdCompiledClass* klass;
    int definition_num_args;
    /// Instance field slots; index matches class-scope symbol index.
    std::vector<Ad_Object*> fields;

    AdCompiledInstance();
    std::string Inspect() override;
    std::string repr() override;
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
    std::string repr() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

class AdRuntimeBoundMethod : public Ad_Object {
public:
    Ad_Object* receiver;
    std::string method_name;

    AdRuntimeBoundMethod();
    AdRuntimeBoundMethod(Ad_Object* receiver, std::string method_name);
    std::string Inspect() override;
	void Print() override;
	Ad_Object_Type Type() override;
	std::string Hash() override;
	Ad_Object* copy(GarbageCollector*) override;
};

#endif