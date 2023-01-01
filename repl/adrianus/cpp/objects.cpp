#include "objects.h"
#include "environment.h"
#include "listobject.h"
#include "listobject.cpp"
#include <sstream>

#define VERBOSE_MEMORY_CLEANUP 0

std::string Ad_Object::Inspect() {
    std::cout << "not implemented Inspect() in subclass\n";
    return "not implemented Inspect() in subclass\n";
}

void Ad_Object::Print() {
    std::cout << "not implemented Print() in subclass\n";
}

Ad_Object_Type Ad_Object::Type() {
    std::cout << "not implemented Type() in subclass\n";
    return OBJ_NULL;
}

std::string Ad_Object::Hash() {
    return "not implementd Hash() in subclass";
}

Ad_Object* Ad_Object::copy() {
    std::cout << "ERROR: copy() not implementend for this type of object\n";
    return NULL;
}

Ad_Null_Object::Ad_Null_Object() {
    type = OBJ_NULL;
    ref_count = 0;
}

Ad_Null_Object::~Ad_Null_Object() {
    // nothind to do here, type and ref_count are automatically deallocated (because they are not pointers)
}

std::string Ad_Null_Object::Inspect() {
    return "null";
}

void Ad_Null_Object::Print() {
    std::cout << "NullObject";
}

Ad_Object_Type Ad_Null_Object::Type() {
   return type;
}

std::string Ad_Null_Object::Hash() {
    return object_type_map[type] + Inspect();
}

Ad_Integer_Object::Ad_Integer_Object() {
    type = OBJ_INT;
    ref_count = 0;
}

Ad_Integer_Object::Ad_Integer_Object(int v) {
    type = OBJ_INT;
    ref_count = 0;
    value = v;
}

Ad_Integer_Object::~Ad_Integer_Object() {
    // nothing to free here: type, ref_count and value are not pointers
}

std::string Ad_Integer_Object::Inspect() {
    std::string out = "";
    out = std::to_string(value);
    return out;
}

void Ad_Integer_Object::Print() {
    std::cout << value;
}

Ad_Object_Type Ad_Integer_Object::Type() {
    return type;
}

std::string Ad_Integer_Object::Hash() {
    return object_type_map[type] + Inspect();
}

Ad_Object* Ad_Integer_Object::copy() {
    Ad_Integer_Object* new_object = new Ad_Integer_Object(value);
    return new_object;
}

Ad_Float_Object::Ad_Float_Object() {
    // TODO: implement this
}

Ad_Float_Object::Ad_Float_Object(float v) {
    // TODO: implement this
}

Ad_Float_Object::~Ad_Float_Object() {
    // TODO: implement this
}

std::string Ad_Float_Object::Inspect() {
    // TODO: implement this
    return "todo: implement this";
}

void Ad_Float_Object::Print() {
    // TODO: implement this
}

Ad_Object_Type Ad_Float_Object::Type() {
    // TODO: implement this
    return type;
}

std::string Ad_Float_Object::Hash() {
    // TODO: implement this
    return "todo: implement this";
}

Ad_Object* Ad_Float_Object::copy() {
    // TODO: implement this
    return NULL;
}

Ad_ReturnValue_Object::Ad_ReturnValue_Object() {
    type = OBJ_RETURN_VALUE;
    ref_count = 0;
}

Ad_ReturnValue_Object::~Ad_ReturnValue_Object() {
    //free_Ad_Object_memory(value);
}

std::string Ad_ReturnValue_Object::Inspect() {
    // TODO: fix this
    //return "todo: implement inspect in Ad_ReturnValue_Object";
    std::string out = "";
    out = value->Inspect();
    return out;
}

void Ad_ReturnValue_Object::Print() {
    std::cout << value;
}

Ad_Object_Type Ad_ReturnValue_Object::Type() {
    return type;
}

std::string Ad_ReturnValue_Object::Hash() {
    return object_type_map[type] + Inspect();
}

Ad_Boolean_Object::Ad_Boolean_Object() {
    type = OBJ_BOOL;
    ref_count = 0;
}

Ad_Boolean_Object::Ad_Boolean_Object(bool v) {
    type = OBJ_BOOL;
    ref_count = 0;
    value = v;
}

std::string Ad_Boolean_Object::Inspect() {
    std::string out = "";
    if (value) {
        out = "true"; // this needs to be the token literal for true, i should do: out = token_type_map[TT_TRUE] and lower it
    } else {
        out = "false"; // this needs to be the token literal for false, i should do: out = token_type_map[TT_FALSE] and lower it
    }
    return out;
}

void Ad_Boolean_Object::Print() {
    std::cout << value;
}

Ad_Object_Type Ad_Boolean_Object::Type() {
    return type;
}

std::string Ad_Boolean_Object::Hash() {
    return object_type_map[type] + Inspect();
}

Ad_Function_Object::Ad_Function_Object() {
    type = OBJ_FUNCTION;
    ref_count = 0;
}

Ad_Function_Object::Ad_Function_Object(std::vector<Ad_AST_Node*> p, Ad_AST_Node* b, Environment* e) {
    type = OBJ_FUNCTION;
    ref_count = 0;
    params = p;
    body = b;
    env = e;

    if (body) {
        Ad_INCREF(body);
    }
    for (std::vector<Ad_AST_Node*>::iterator it = params.begin() ; it != params.end(); ++it) {
        Ad_AST_Node *obj = *it;
        Ad_INCREF(obj);
    }
}

Ad_Function_Object::~Ad_Function_Object() {
    if (body) {
        Ad_DECREF(body);
        free_Ad_AST_Node_memory(body);
    }
    for (std::vector<Ad_AST_Node*>::iterator it = params.begin() ; it != params.end(); ++it) {
        Ad_AST_Node *obj = *it;
        Ad_DECREF(obj);
        free_Ad_AST_Node_memory(obj);
    }
    //Ad_DECREF(env);
}

std::string Ad_Function_Object::Inspect() {
    std::stringstream ss;
    ss << std::hex << this;
    return "function at memory address " + ss.str();
}

void Ad_Function_Object::Print() {
    std::cout << "todo: implement Print in Ad_Function_Object\n";
}

Ad_Object_Type Ad_Function_Object::Type() {
    return type;
}

std::string Ad_Function_Object::Hash() {
    return object_type_map[type] + Inspect();;
}

Ad_String_Object::Ad_String_Object() {
    type = OBJ_STRING;
    ref_count = 0;
}

Ad_String_Object::Ad_String_Object(std::string val) {
    type = OBJ_STRING;
    ref_count = 0;
    value = val;
}

std::string Ad_String_Object::Inspect() {
    return "\"" + value + "\"";
}

void Ad_String_Object::Print() {
    std::cout << "todo Ad string Print()";
}

Ad_Object_Type Ad_String_Object::Type() {
    return type;
}

std::string Ad_String_Object::Hash() {
    return object_type_map[type] + Inspect();
}

Ad_Error_Object::Ad_Error_Object() {
    type = OBJ_ERROR;
    ref_count = 0;
}

Ad_Error_Object::Ad_Error_Object(std::string m) {
    type = OBJ_ERROR;
    ref_count = 0;
    message = m;
}

std::string Ad_Error_Object::Inspect() {
    std::string out;
    out = "ERROR: " + message;
    return out;
}

void Ad_Error_Object::Print() {
    std::cout << message;
}

Ad_Object_Type Ad_Error_Object::Type() {
    return type;
}

std::string Ad_Error_Object::Hash() {
    return object_type_map[type] + Inspect();
}

Ad_Builtin_Object::Ad_Builtin_Object() {
    type = OBJ_BUILTIN;
    ref_count = 0;
}

Ad_Builtin_Object::Ad_Builtin_Object(BuiltinFunction bf) {
    type = OBJ_BUILTIN;
    ref_count = 0;
    builtin_function = bf;
}

Ad_Builtin_Object::Ad_Builtin_Object(BuiltinFunction bf, std::vector<int> anoa) {
    type = OBJ_BUILTIN;
    ref_count = 0;
    builtin_function = bf;
    acceptedNumbersOfArguments = anoa;
}

Ad_Builtin_Object::~Ad_Builtin_Object() {

}

std::string Ad_Builtin_Object::Inspect() {
    return "todo: implement Ad_Builtin_Object::Inspect()";
}

void Ad_Builtin_Object::Print() {
    std::cout << "todo: implement Ad_Builtin_Object::Print()\n";
}

Ad_Object_Type Ad_Builtin_Object::Type() {
    return type;
}

std::string Ad_Builtin_Object::Hash() {
    return object_type_map[type] + Inspect();
}

Ad_Signal_Object::Ad_Signal_Object() {
    type = OBJ_SIGNAL;
    ref_count = 0;
}

std::string Ad_Signal_Object::Inspect() {
    return "todo: implement Inspect in Ad_Signal_Object";
}

void Ad_Signal_Object::Print() {
    std::cout << "todo: implement Print in Ad_Signal_Object";
}

Ad_Object_Type Ad_Signal_Object::Type() {
    return type;
}

std::string Ad_Signal_Object::Hash() {
    return object_type_map[type] + Inspect();
}

Ad_List_Object::Ad_List_Object() {
    type = OBJ_LIST;
    ref_count = 0;
    //elements = new Ad_List();
}

Ad_List_Object::Ad_List_Object(std::vector<Ad_Object*> e) {
    type = OBJ_LIST;
    ref_count = 0;
    elements = e;
    // this is not efficient
    for (std::vector<Ad_Object*>::iterator it = elements.begin() ; it != elements.end(); ++it) {
        Ad_INCREF(*it);
    }
}

Ad_List_Object::~Ad_List_Object() {
    for (std::vector<Ad_Object*>::iterator it = elements.begin() ; it != elements.end(); ++it) {
        Ad_Object* obj = *it;
        Ad_DECREF(obj);
        free_Ad_Object_memory(obj);
    }
}

std::string Ad_List_Object::Inspect() {
    std::string out = "[";
    int idx = 0;
    for (std::vector<Ad_Object*>::iterator it = elements.begin() ; it != elements.end(); ++it) {
        Ad_Object *obj = *it;
        if (idx++ > 0) out += ", ";
        out += obj->Inspect();
    }
    out += "]";
    return out;
}

void Ad_List_Object::Print() {
    //std::cout << elements->Inspect();
}

Ad_Object_Type Ad_List_Object::Type() {
    return type;
}

std::string Ad_List_Object::Hash() {
    return object_type_map[type] + Inspect();
}

Ad_Object* Ad_List_Object::copy() {
    std::vector<Ad_Object*> newElements;
    for (std::vector<Ad_Object*>::iterator it = elements.begin() ; it != elements.end(); ++it) {
        newElements.push_back((*it)->copy());
    }

    Ad_List_Object* new_object = new Ad_List_Object(newElements);
    return new_object;
}

Ad_Hash_Object::Ad_Hash_Object() {
    type = OBJ_HASH;
    ref_count = 0;
}

Ad_Hash_Object::Ad_Hash_Object(std::map<std::string, HashPair> p) {
    type = OBJ_HASH;
    ref_count = 0;
    pairs = p;
}

Ad_Hash_Object::~Ad_Hash_Object() {
    for(std::map<std::string, HashPair>::iterator it = pairs.begin(); it != pairs.end(); it++) {
        Ad_DECREF(it->second.key);
        Ad_DECREF(it->second.value);
        free_Ad_Object_memory(it->second.key);
        free_Ad_Object_memory(it->second.value);
    }
}

std::string Ad_Hash_Object::Inspect() {
    std::string out = "{";
    bool displayed_first = false;
    for(std::map<std::string, HashPair>::iterator it = pairs.begin(); it != pairs.end(); it++) {
        if (displayed_first) out += ", ";
        else displayed_first = true;
        out += it->second.key->Inspect() + ": " + it->second.value->Inspect();
    }
    out += "}";
    return out;
}

void Ad_Hash_Object::Print() {
    std::cout << "todo: implement Print() in Ad_Hash_Object\n";
}

Ad_Object_Type Ad_Hash_Object::Type() {
    return type;
}

std::string Ad_Hash_Object::Hash() {
    return object_type_map[type] + Inspect();
}

Ad_Class_Object::Ad_Class_Object() {
    type = OBJ_CLASS;
    ref_count = 0;
    attemptASTNodesDeletion = false;
}

Ad_Class_Object::Ad_Class_Object(std::vector<Ad_AST_Node*> m, std::vector<Ad_AST_Node*> a) {
    type = OBJ_CLASS;
    ref_count = 0;
    attemptASTNodesDeletion = false;
    methods = m;
    for (std::vector<Ad_AST_Node*>::iterator it = methods.begin() ; it != methods.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_INCREF(node);
    }
    attributes = a;
    for (std::vector<Ad_AST_Node*>::iterator it = attributes.begin() ; it != attributes.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_INCREF(node);
    }
}

Ad_Class_Object::Ad_Class_Object(Ad_AST_Node* n, std::vector<Ad_AST_Node*> m, std::vector<Ad_AST_Node*> a) {
    type = OBJ_CLASS;
    ref_count = 0;
    attemptASTNodesDeletion = false;
    name = n;
    Ad_INCREF(name);
    methods = m;
    for (std::vector<Ad_AST_Node*>::iterator it = methods.begin(); it != methods.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_INCREF(node);
    }
    attributes = a;
    for (std::vector<Ad_AST_Node*>::iterator it = attributes.begin(); it != attributes.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_INCREF(node);
    }
}

Ad_Class_Object::Ad_Class_Object(Ad_AST_Node* n, std::vector<Ad_AST_Node*> m, std::vector<Ad_AST_Node*>a, Ad_AST_Node* c) {
    type = OBJ_CLASS;
    ref_count = 0;
    attemptASTNodesDeletion = false;
    class_ast_node = c;
    Ad_INCREF(class_ast_node);

    name = n;
    Ad_INCREF(name);
    methods = m;
    for (std::vector<Ad_AST_Node*>::iterator it = methods.begin(); it != methods.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_INCREF(node);
    }
    attributes = a;
    for (std::vector<Ad_AST_Node*>::iterator it = attributes.begin(); it != attributes.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_INCREF(node);
    }
}

Ad_Class_Object::~Ad_Class_Object() {
    //std::cout << "deleting class" << ((Ad_AST_Identifier*)name)->value << "\n";
    //std::cout << is_console_run << "\n";
    Ad_DECREF(class_ast_node);
    if (attemptASTNodesDeletion) {
        free_Ad_AST_Node_memory(class_ast_node);
    }

    // if i deallocate the ast node that references all the methods and attributes ast, do i really need to try and manually free them again?
    for (std::vector<Ad_AST_Node*>::iterator it = methods.begin() ; it != methods.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_DECREF(node); // asta merge si e super cool
        if (attemptASTNodesDeletion) {
            free_Ad_AST_Node_memory(node);
        }
    }
    for (std::vector<Ad_AST_Node*>::iterator it = attributes.begin() ; it != attributes.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_DECREF(node); // asta merge si e super cool
        if (attemptASTNodesDeletion) {
            free_Ad_AST_Node_memory(node);
        }
    }
}

void Ad_Class_Object::deleteASTNodeFromBootstrapEnvironment() { // this is unused anymore
    // this is just like the destructor, if just does the delete also
    Ad_DECREF(class_ast_node);
    free_Ad_AST_Node_memory(class_ast_node);

    // if i deallocate the ast node that references all the methods and attributes ast, do i really need to try and manually free them again?
    for (std::vector<Ad_AST_Node*>::iterator it = methods.begin() ; it != methods.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_DECREF(node); // asta merge si e super cool
        free_Ad_AST_Node_memory(node);
    }
    for (std::vector<Ad_AST_Node*>::iterator it = attributes.begin() ; it != attributes.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_DECREF(node); // asta merge si e super cool
        free_Ad_AST_Node_memory(node);
    }
}

std::string Ad_Class_Object::Inspect() {
    std::stringstream ss;
    ss << std::hex << this;
    return "class object at memory address " + ss.str();
}

void Ad_Class_Object::Print() {
    std::cout << "ClassObject\n";
    //std::cout << methods.size();
    //std::cout << attributes.size();
}

Ad_Object_Type Ad_Class_Object::Type() {
    return type;
}

std::string Ad_Class_Object::Hash() {
    return object_type_map[type] + Inspect();
}

Ad_Class_Instance::Ad_Class_Instance() {
    type = OBJ_INSTANCE;
    ref_count = 0;
}

Ad_Class_Instance::Ad_Class_Instance(Ad_Object* ko, Environment* env) {
    type = OBJ_INSTANCE;
    ref_count = 0;
    klass_object = ko;
    instance_environment = env;
}

Ad_Class_Instance::Ad_Class_Instance(std::string n, Ad_Object* ko, Environment* env) {
    type = OBJ_INSTANCE;
    ref_count = 0;
    name = n;
    klass_object = ko;
    instance_environment = env;
}

Ad_Class_Instance::~Ad_Class_Instance() {
    if (instance_environment) {
        delete instance_environment;
    }
}

std::string Ad_Class_Instance::Inspect() {
    std::stringstream ss;
    ss << std::hex << this;
    return "class instance at memory address " + ss.str();
}

void Ad_Class_Instance::Print() {
    std::cout << "todo: implement Print() in Ad_Class_Instance\n";
}

Ad_Object_Type Ad_Class_Instance::Type() {
    return type;
}

std::string Ad_Class_Instance::Hash() {
    return object_type_map[type] + Inspect();
}

Ad_File_Object::Ad_File_Object() {
    type = OBJ_FILE;
    ref_count = 0;
}

Ad_File_Object::Ad_File_Object(std::string f, std::string o) {
    type = OBJ_FILE;
    ref_count = 0;
    filename = f;
    _operator = o;
}

Ad_File_Object::~Ad_File_Object() {

}

std::string Ad_File_Object::Inspect() {
    std::string out;
    out = "File [" + filename + ", " + _operator + "]";
    return out;
}

void Ad_File_Object::Print() {
    std::cout << "todo: implement Print() in Ad_File_Object\n";
}

Ad_Object_Type Ad_File_Object::Type() {
    return type;
}

std::string Ad_File_Object::Hash() {
    return object_type_map[type] + Inspect();
}

Ad_Break_Object::Ad_Break_Object() {
    type = OBJ_BREAK;
    ref_count = 0;
}

Ad_Break_Object::~Ad_Break_Object() {

}

std::string Ad_Break_Object::Inspect() {
    return NULL;
}

void Ad_Break_Object::Print() {

}

Ad_Object_Type Ad_Break_Object::Type() {
    return type;
}

std::string Ad_Break_Object::Hash() {
    return NULL;
}

Ad_Continue_Object::Ad_Continue_Object() {
    type = OBJ_CONTINUE;
    ref_count = 0;
}

Ad_Continue_Object::~Ad_Continue_Object() {

}

std::string Ad_Continue_Object::Inspect() {
    return NULL;
}

void Ad_Continue_Object::Print() {

}

Ad_Object_Type Ad_Continue_Object::Type() {
    return type;
}

std::string Ad_Continue_Object::Hash() {
    return NULL;
}

Ad_Socket_Object::Ad_Socket_Object() {
    type = OBJ_SOCKET;
    ref_count = 0;
}

/*Ad_Socket_Object::Ad_Socket_Object(std::string n) {
    type = OBJ_SOCKET;
    ref_count = 0;
    name = n;
}

Ad_Socket_Object::Ad_Socket_Object(std::string n, int p) {
    type = OBJ_SOCKET;
    ref_count = 0;
    name = n;
    port = p;
}*/

Ad_Socket_Object::Ad_Socket_Object(std::string n, int p, bool a, bool f, bool c, bool s) {
    type = OBJ_SOCKET;
    ref_count = 0;
    name = n;
    port = p;
    isActive = a;
    isForever = f;
    isClient = c;
    isServer = s;
}

Ad_Socket_Object::~Ad_Socket_Object() {
    // TODO: implement this
}

std::string Ad_Socket_Object::Inspect() {
    // return "<socket at address 0x0>"; // maybe this?
    //std::stringstream ss;
    //ss << "Socket [name=" << name << ", port=" << port << ", isActive=" << isActive << ", isForever" << isForever << ", isClient=" << isClient << ", isServer=" << isServer << "]";
    //return ss.str();
    std::stringstream ss;
    ss << std::hex << this;
    return "<socket instance at memory address " + ss.str() + ">";
}

void Ad_Socket_Object::Print() {
    std::cout << "SocketObject\n";
}

Ad_Object_Type Ad_Socket_Object::Type() {
    return type;
}

std::string Ad_Socket_Object::Hash() {
    return object_type_map[type] + Inspect();
}

void Ad_INCREF(Ad_Object* obj) {
    if (obj) {
        obj->ref_count++;
    }
}

void Ad_DECREF(Ad_Object* obj) {
    if (obj) {
        obj->ref_count--;
    }
}

void free_Ad_Object_memory(Ad_Object* obj) {
    if (obj == NULL) return;
    if (obj->ref_count > 0) return;
    if (VERBOSE_MEMORY_CLEANUP) {
        std::cout << "freeing up some memory\n";
    }
    if (obj) {
        switch(obj->type) {
            case OBJ_NULL:
                if (!((Ad_Null_Object*)obj)->permanent) {
                    delete ((Ad_Null_Object*)obj);
                }
            break;
            case OBJ_INT:
                delete ((Ad_Integer_Object*)obj);
            break;
            case OBJ_BOOL:
                if (!((Ad_Boolean_Object*)obj)->permanent) {
                    delete ((Ad_Boolean_Object*)obj);
                }
            break;
            case OBJ_STRING:
                delete ((Ad_String_Object*)obj);
            break;
            case OBJ_RETURN_VALUE:
                delete ((Ad_ReturnValue_Object*)obj);
            break;
            case OBJ_FUNCTION:
                delete ((Ad_Function_Object*)obj);
            break;
            case OBJ_ERROR:
                delete ((Ad_Error_Object*)obj);
            break;
            case OBJ_BUILTIN:
                delete ((Ad_Builtin_Object*)obj);
            break;
            case OBJ_SIGNAL:
                delete ((Ad_Signal_Object*)obj);
            break;
            case OBJ_LIST:
                delete ((Ad_List_Object*)obj);
            break;
            case OBJ_HASH:
                delete ((Ad_Hash_Object*)obj);
            break;
            case OBJ_CLASS:
                //std::cout << "deleting a class object " << obj << "\n";
                delete (Ad_Class_Object*) obj;
            break;
            case OBJ_INSTANCE:
                delete (Ad_Class_Instance*) obj;
            break;
            case OBJ_FILE:
                delete (Ad_File_Object*) obj;
            break;
            case OBJ_BREAK:
                delete (Ad_Break_Object*) obj;
            break;
            case OBJ_CONTINUE:
                delete (Ad_Continue_Object*) obj;
            break;
            default:
                std::cout << obj->type << "\n";
                std::cout << "MEMORY ERROR!!! object: " << object_type_map[obj->type] << "\n";
            break;
        }
    }
}

void print_Ad_Object(Ad_Object* obj) {
    if (obj == NULL) {
        std::cout << "oops... something went wrong";
        return;
    }
    switch(obj->type) {
        case OBJ_NULL:
            std::cout << "null\n";
        break;
        case OBJ_INT:
            std::cout << ((Ad_Integer_Object*)obj)->value << "\n";
        break;
        case OBJ_BOOL:
            std::cout << ((Ad_Boolean_Object*)obj)->value << "\n";
        break;
        case OBJ_STRING:
            std::cout << ((Ad_String_Object*)obj)->value << "\n";
        break;
        case OBJ_RETURN_VALUE:
            std::cout << ((Ad_ReturnValue_Object*)obj)->value << "\n";
        break;
        case OBJ_FUNCTION:
            std::cout << "Function object\n";
        break;
        case OBJ_ERROR:
            std::cout << "error object\n";
        break;
        case OBJ_BUILTIN:
            std::cout << "builtin object\n";
        break;
        case OBJ_SIGNAL:
            std::cout << "signal object\n";
        break;
        case OBJ_CLASS:
            std::cout << "class object\n";
        break;
        case OBJ_INSTANCE:
            std::cout << "class instance\n";
        break;
        case OBJ_FILE:
            std::cout << "file object\n";
        break;
        default:
            std::cout << "no print implmentation for this object type";
        break;
    }
}
