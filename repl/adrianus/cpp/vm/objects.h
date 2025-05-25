#ifndef __VM_OBJECTS_H
#define __VM_OBJECTS_H

class AdCompiledFunction : public Ad_Object {
public:
    AdCompiledFunction();
    virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*);
};

class AdClosureObject : public Ad_Object {
public:
	AdClosureObject();
    virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*);
};

class AdCompiledClass : public Ad_Object {
public:
    AdCompiledClass();
    virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*);
};

class AdCompiledInstance : public Ad_Object {
public:
    AdCompiledInstance();
    virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*);
};

class AdBoundMethod : public Ad_Object {
public:
    AdBoundMethod();
    virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*);
};

#endif