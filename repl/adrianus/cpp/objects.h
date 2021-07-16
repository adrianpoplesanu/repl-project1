#ifndef __OBJECTS_H
#define __OBJECTS_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "ast.h"

enum Ad_Object_Type {
	OBJ_NULL,
	OBJ_INT,
	OBJ_BOOL,
	OBJ_STRING,
	OBJ_RETURN_VALUE,
	OBJ_FUNCTION,
	OBJ_ERROR,
	OBJ_BUILTIN
};

std::map<Ad_Object_Type, std::string> object_type_map = {
	{OBJ_NULL, "NullObject"},
	{OBJ_INT, "IntObject"},
	{OBJ_BOOL, "BoolObject"},
	{OBJ_STRING, "StringObject"},
	{OBJ_RETURN_VALUE, "ReturnObject"},
	{OBJ_FUNCTION, "FunctionObject"},
	{OBJ_STRING, "StringObject"},
	{OBJ_ERROR, "ErrorObject"},
	{OBJ_BUILTIN, "BuiltinObject"}
};

class Ad_Object {
public:
	Ad_Object_Type type;
	unsigned int num_octets;
	unsigned int ref_count;
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
};

class Ad_Null_Object : public Ad_Object {
public:
	Ad_Null_Object();
};

class Ad_Integer_Object : public Ad_Object {
public:
	int value;

	Ad_Integer_Object();
	Ad_Integer_Object(int);
	~Ad_Integer_Object();
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
};

class Ad_Boolean_Object : public Ad_Object {
public:
	bool value;

	Ad_Boolean_Object();
	Ad_Boolean_Object(bool);
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
};

class Ad_String_Object : public Ad_Object {
public:
	std::string value;

	Ad_String_Object();
	Ad_String_Object(std::string);
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
};

class Ad_ReturnValue_Object : public Ad_Object {
public:
	Ad_Object* value;

	Ad_ReturnValue_Object();
	Ad_ReturnValue_Object(Ad_Object*);

	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
};

class Environment; // forward declaration for Environment

class Ad_Function_Object : public Ad_Object {
public:
	std::vector<Ad_AST_Node*> params; // i don't really like using AST nodes here
	Ad_AST_Node* body; // i don't really like using AST nodes here
	Environment *env; // this works only with forward declaration

	Ad_Function_Object();
	Ad_Function_Object(std::vector<Ad_AST_Node*>, Ad_AST_Node*, Environment*);
	//~Ad_Function_Object();

	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
};

class Ad_Error_Object : public Ad_Object {
public:
	std::string message;

	Ad_Error_Object();
	Ad_Error_Object(std::string);
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
};

void Ad_INCREF(Ad_Object*);
void Ad_DECREF(Ad_Object*);

void free_Ad_Object_memory(Ad_Object*);
void print_Ad_Object(Ad_Object*);

class Ad_Builtin_Object : public Ad_Object {
public:
	//...
	Ad_Object* (*builtin_function)(std::vector<Ad_Object*>);

	Ad_Builtin_Object();
	Ad_Builtin_Object(Ad_Object* (*)(std::vector<Ad_Object*>));
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
};

#endif