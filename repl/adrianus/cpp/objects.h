#ifndef __OBJECTS_H
#define __OBJECTS_H

#include <iostream>
#include <string>

enum Ad_Object_Type {
	OBJ_NULL,
	OBJ_INT,
	OBJ_BOOL,
	OBJ_STRING,
	OBJ_RETURN_VALUE
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

void Ad_INCREF(Ad_Object*);
void Ad_DECREF(Ad_Object*);

#endif