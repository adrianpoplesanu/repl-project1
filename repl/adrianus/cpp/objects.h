#ifndef __OBJECTS_H
#define __OBJECTS_H

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include "ast.h"
#include "signal.h"
#include "hashpair.h"
#include "settings.h"
#include "gc.h"
#include <thread>

enum Ad_Object_Type {
	OBJ_NULL,
	OBJ_INT,
	OBJ_FLOAT,
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
	OBJ_SOCKET,
	OBJ_THREAD,
	OBJ_BREAK,
	OBJ_CONTINUE
};

std::unordered_map<Ad_Object_Type, std::string> object_type_map = {
	{OBJ_NULL, "NULL"},
	{OBJ_INT, "INTEGER"},
	{OBJ_FLOAT, "FLOAT"},
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
	{OBJ_SOCKET, "SOCKET"},
	{OBJ_THREAD, "THREAD"},
	{OBJ_BREAK, "BREAK"},
	{OBJ_CONTINUE, "CONTINUE"}
};

class Ad_Object {
public:
	Ad_Object_Type type;
	unsigned int num_octets;
	unsigned int ref_count;
	bool marked;
	Ad_Object* next;
	Ad_Object* prev;
	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*);
};

class Ad_Null_Object : public Ad_Object {
public:
    bool permanent;

	Ad_Null_Object();
	~Ad_Null_Object();
	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(GarbageCollector*); // TODO: implement this
};

class Ad_Integer_Object : public Ad_Object {
public:
	int value;

	Ad_Integer_Object();
	Ad_Integer_Object(int);
	~Ad_Integer_Object();
	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*);
};

class Ad_Float_Object : public Ad_Object {
public:
	float value;

	Ad_Float_Object();
	Ad_Float_Object(float);
	~Ad_Float_Object();
	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*);
};

class Ad_Boolean_Object : public Ad_Object {
public:
	bool value;
	bool permanent;

	Ad_Boolean_Object();
	Ad_Boolean_Object(bool);
	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(GarbageCollector*); // TODO: implement this
};

class Ad_String_Object : public Ad_Object {
public:
	std::string value;

	Ad_String_Object();
	Ad_String_Object(std::string);
	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*);
};

class Ad_ReturnValue_Object : public Ad_Object {
public:
	Ad_Object* value;

	Ad_ReturnValue_Object();
	Ad_ReturnValue_Object(Ad_Object*);
	~Ad_ReturnValue_Object();

	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(GarbageCollector*); // TODO: implement this
};

class Ad_Break_Object : public Ad_Object {
public:
	Ad_Break_Object();
	~Ad_Break_Object();

	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
};

class Ad_Continue_Object : public Ad_Object {
public:
	Ad_Continue_Object();
	~Ad_Continue_Object();

	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();	
};

class Environment; // forward declaration for Environment

class Ad_Function_Object : public Ad_Object {
public:
	std::vector<Ad_AST_Node*> params; // i don't really like using AST nodes here
	std::vector<Ad_AST_Node*> default_params;
	Ad_AST_Node* body; // i don't really like using AST nodes here
	Environment* env; // this works only with forward declaration

	Ad_Function_Object();
	Ad_Function_Object(std::vector<Ad_AST_Node*>, Ad_AST_Node*, Environment*);
	Ad_Function_Object(std::vector<Ad_AST_Node*>, std::vector<Ad_AST_Node*>, Ad_AST_Node*, Environment*);
	~Ad_Function_Object();

	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*);
};

class Ad_Error_Object : public Ad_Object {
public:
	std::string message;

	Ad_Error_Object();
	Ad_Error_Object(std::string);
	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(GarbageCollector*); // TODO: implement this
};

class Ad_Builtin_Object : public Ad_Object {
public:
	typedef Ad_Object* (*BuiltinFunction)(std::vector<Ad_Object*>, Environment*, GarbageCollector*);

	std::vector<int> acceptedNumbersOfArguments;
	BuiltinFunction builtin_function;

	Ad_Builtin_Object();
	Ad_Builtin_Object(BuiltinFunction);
	Ad_Builtin_Object(BuiltinFunction, std::vector<int>);
	~Ad_Builtin_Object();
	virtual std::string Inspect();
    virtual std::string repr();
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
    virtual std::string repr();
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
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*);
};

class Ad_Hash_Object : public Ad_Object {
public:
	std::unordered_map<std::string, HashPair> pairs; // do i need a HashPair implementation also? the answer is yes, i do need it

	Ad_Hash_Object();
	Ad_Hash_Object(std::unordered_map<std::string, HashPair>);
	~Ad_Hash_Object();
	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*);
};

class Ad_Class_Object : public Ad_Object {
public:
	Ad_AST_Node* class_ast_node;
	Ad_AST_Node* name;
	std::vector<Ad_AST_Node*> methods;
	std::vector<Ad_AST_Node*> attributes;
	std::vector<Ad_AST_Node*> inheritFrom;
	bool attemptASTNodesDeletion;

	Ad_Class_Object();
	Ad_Class_Object(std::vector<Ad_AST_Node*>, std::vector<Ad_AST_Node*>);
	Ad_Class_Object(Ad_AST_Node*, std::vector<Ad_AST_Node*>, std::vector<Ad_AST_Node*>);
	Ad_Class_Object(Ad_AST_Node*, std::vector<Ad_AST_Node*>, std::vector<Ad_AST_Node*>, Ad_AST_Node*);
	~Ad_Class_Object();
	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	void deleteASTNodeFromBootstrapEnvironment(); // maybe use a gc pool here for ast nodes? - the method name si so specific, i don't like it
	//virtual Ad_Object* copy(GarbageCollector*); // TODO: implement this
};

class Ad_Class_Instance : public Ad_Object {
public:
	std::string name; // this might not be needed
	Ad_Object* klass_object;
    Environment* instance_environment;
	std::vector<std::string> inheritFrom;

	Ad_Class_Instance();
	Ad_Class_Instance(Ad_Object*, Environment*);
	Ad_Class_Instance(std::string, Ad_Object*, Environment*);
	~Ad_Class_Instance();
	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*);
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
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	//virtual Ad_Object* copy(GarbageCollector*); // TODO: implement this
};

class Ad_Socket_Object : public Ad_Object {
public:
	std::string name;
	std::string host;
	int port;
	bool isActive;
	bool isForever;
	bool isClient;
	bool isServer;
	int listenfd; // listen file descriptor
	int connfd = 0; // connection file descriptor
	char sendBuff[1024];
	char recvBuff[1024];

	Ad_Socket_Object();
	Ad_Socket_Object(std::string, std::string, int, bool, bool, bool, bool);
	~Ad_Socket_Object();
	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*); // TODO: implement this
};

class Ad_Thread_Object : public Ad_Object {
public:
	std::string name;
	Ad_Object* callback;
	Ad_Object* result;
	std::vector<Ad_Object*> params;
	std::thread *internal_thread;
	GarbageCollector *internal_gc;

	Ad_Thread_Object();
	~Ad_Thread_Object();
	virtual std::string Inspect();
    virtual std::string repr();
	virtual void Print();
	virtual Ad_Object_Type Type();
	virtual std::string Hash();
	virtual Ad_Object* copy(GarbageCollector*); // TODO: implement this
};

//void Ad_INCREF(Ad_Object*);
//void Ad_DECREF(Ad_Object*);

void free_Ad_Object_memory(Ad_Object*);
void print_Ad_Object(Ad_Object*);

Ad_Null_Object NULLOBJECT;

#endif