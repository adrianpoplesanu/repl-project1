#ifndef __OBJECTS_H
#define __OBJECTS_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include "ast.h"
#include "signal.h"
#include "hashpair.h"

enum Ad_Object_Type {
	OBJ_NULL,
	OBJ_INT,
	OBJ_BOOL,
	OBJ_STRING,
	OBJ_RETURN_VALUE,
	OBJ_FUNCTION,
	OBJ_ERROR,
	OBJ_BUILTIN,
	OBJ_SIGNAL,
	OBJ_LIST,
	OBJ_HASH,
	OBJ_CLASS,
	OBJ_INSTANCE,
	OBJ_FILE,
	OBJ_SOCKET
};

std::map<Ad_Object_Type, std::string> object_type_map = {
	{OBJ_NULL, "NULL"},
	{OBJ_INT, "INTEGER"},
	{OBJ_BOOL, "BOOLEAN"},
	{OBJ_STRING, "STRING"},
	{OBJ_RETURN_VALUE, "RETURN_VALUE"},
	{OBJ_FUNCTION, "FUNCTION"},
	{OBJ_ERROR, "ERROR"},
	{OBJ_BUILTIN, "BUILTIN"},
	{OBJ_SIGNAL, "SIGNAL"},
	{OBJ_LIST, "LIST"},
	{OBJ_HASH, "HASH"},
	{OBJ_CLASS, "CLASS"},
	{OBJ_INSTANCE, "INSTANCE"},
	{OBJ_FILE, "FILE"},
	{OBJ_SOCKET, "SOCKET"}
};

class Ad_Object {
public:
	Ad_Object_Type type;
	unsigned int num_octets;
	unsigned int ref_count;
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy();
};

class Ad_Null_Object : public Ad_Object {
public:
    bool permanent;

	Ad_Null_Object();
	~Ad_Null_Object();
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(); // TODO: implement this
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
	virtual std::string Hash();
	virtual Ad_Object* copy();
};

class Ad_Boolean_Object : public Ad_Object {
public:
	bool value;
	bool permanent;

	Ad_Boolean_Object();
	Ad_Boolean_Object(bool);
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(); // TODO: implement this
};

class Ad_String_Object : public Ad_Object {
public:
	std::string value;

	Ad_String_Object();
	Ad_String_Object(std::string);
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(); // TODO: implement this
};

class Ad_ReturnValue_Object : public Ad_Object {
public:
	Ad_Object* value;

	Ad_ReturnValue_Object();
	Ad_ReturnValue_Object(Ad_Object*);
	~Ad_ReturnValue_Object();

	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(); // TODO: implement this
};

class Environment; // forward declaration for Environment

class Ad_Function_Object : public Ad_Object {
public:
	std::vector<Ad_AST_Node*> params; // i don't really like using AST nodes here
	Ad_AST_Node* body; // i don't really like using AST nodes here
	Environment* env; // this works only with forward declaration

	Ad_Function_Object();
	Ad_Function_Object(std::vector<Ad_AST_Node*>, Ad_AST_Node*, Environment*);
	~Ad_Function_Object();

	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(); // TODO: implement this
};

class Ad_Error_Object : public Ad_Object {
public:
	std::string message;

	Ad_Error_Object();
	Ad_Error_Object(std::string);
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(); // TODO: implement this
};

class Ad_Builtin_Object : public Ad_Object {
public:
	//...
	Ad_Object* (*builtin_function)(std::vector<Ad_Object*>, Environment*);

	Ad_Builtin_Object();
	Ad_Builtin_Object(Ad_Object* (*)(std::vector<Ad_Object*>, Environment*));
	~Ad_Builtin_Object();
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(); // TODO: implement this
};

class Ad_Signal_Object : public Ad_Object {
public:
	Ad_Signal_Type signal_type;

	Ad_Signal_Object();
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(); // TODO: implement this
};

//class Ad_List;

class Ad_List_Object : public Ad_Object {
public:
	//Ad_List *elements; // forward declaration
	std::vector<Ad_Object*> elements;

	Ad_List_Object();
	Ad_List_Object(std::vector<Ad_Object*>);
	~Ad_List_Object();
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(); // TODO: implement this
};

class Ad_Hash_Object : public Ad_Object {
public:
	std::map<std::string, HashPair> pairs; // do i need a HashPair implementation also? the answer is yes, i do need it

	Ad_Hash_Object();
	Ad_Hash_Object(std::map<std::string, HashPair>);
	~Ad_Hash_Object();
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(); // TODO: implement this
};

class Ad_Class_Object : public Ad_Object {
public:
	Ad_AST_Node* class_ast_node;
	Ad_AST_Node* name;
	std::vector<Ad_AST_Node*> methods;
	std::vector<Ad_AST_Node*> attributes;

	Ad_Class_Object();
	Ad_Class_Object(std::vector<Ad_AST_Node*>, std::vector<Ad_AST_Node*>);
	Ad_Class_Object(Ad_AST_Node*, std::vector<Ad_AST_Node*>, std::vector<Ad_AST_Node*>);
	Ad_Class_Object(Ad_AST_Node*, std::vector<Ad_AST_Node*>, std::vector<Ad_AST_Node*>, Ad_AST_Node*);
	~Ad_Class_Object();
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(); // TODO: implement this
};

class Ad_Class_Instance : public Ad_Object {
public:
	std::string name; // this might not be needed
	Ad_Object* klass_object;
    Environment* instance_environment;

	Ad_Class_Instance();
	Ad_Class_Instance(Ad_Object*, Environment*);
	Ad_Class_Instance(std::string, Ad_Object*, Environment*);
	~Ad_Class_Instance();
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(); // TODO: implement this
};

class Ad_File_Object : public Ad_Object {
public:
	std::string filename;
	std::string _operator;
	std::ifstream input_descriptor;
	std::ofstream output_descriptor;


	Ad_File_Object();
	Ad_File_Object(std::string, std::string);
	~Ad_File_Object();
	virtual std::string Inspect();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(); // TODO: implement this
};

class Ad_Socket_Object : public Ad_Object {
public:
	std::string name;
	int port;
	bool isActive;
};

void Ad_INCREF(Ad_Object*);
void Ad_DECREF(Ad_Object*);

void free_Ad_Object_memory(Ad_Object*);
void print_Ad_Object(Ad_Object*);

Ad_Null_Object NULLOBJECT;

#endif