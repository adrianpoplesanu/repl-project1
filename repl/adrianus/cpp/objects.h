#ifndef __OBJECTS_H
#define __OBJECTS_H

#include <iostream>
#include <string>
#include <map>

enum Ad_Object_Type {
	OBJ_NULL,
	OBJ_INT,
	OBJ_BOOL,
	OBJ_STRING,
	OBJ_RETURN_VALUE,
	OBJ_ERROR
};

std::map<Ad_Object_Type, std::string> object_type_map = {
	{OBJ_NULL, "NullObject"},
	{OBJ_INT, "IntObject"},
	{OBJ_BOOL, "BoolObject"},
	{OBJ_STRING, "StringObject"},
	{OBJ_RETURN_VALUE, "ReturnObject"},
	{OBJ_ERROR, "ErrorObject"}
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
};

class Ad_ReturnValue_Object : public Ad_Object {
public:
	Ad_Object* value;

	Ad_ReturnValue_Object();
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

#endif