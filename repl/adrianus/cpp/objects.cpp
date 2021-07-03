#include "objects.h"

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

Ad_Null_Object::Ad_Null_Object() {
    type = OBJ_NULL;
}

Ad_Integer_Object::Ad_Integer_Object() {
    type = OBJ_INT;
}

Ad_Integer_Object::Ad_Integer_Object(int v) {
    type = OBJ_INT;
    value = v;
}

Ad_Integer_Object::~Ad_Integer_Object() {
    //std::cout << value << "\n";
    //std::cout << "doing some deletion here\n";
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

Ad_ReturnValue_Object::Ad_ReturnValue_Object() {

}

std::string Ad_ReturnValue_Object::Inspect() {
    // TODO: fix this
    return "todo: implement inspect in Ad_ReturnValue_Object";
    /*std::string out = "";
    out = std::to_string(value->value);
    return out;*/
}

void Ad_ReturnValue_Object::Print() {
    std::cout << value;
}

Ad_Object_Type Ad_ReturnValue_Object::Type() {
    return type;
}

Ad_Boolean_Object::Ad_Boolean_Object() {
    type = OBJ_BOOL;
}

Ad_Boolean_Object::Ad_Boolean_Object(bool v) {
    type = OBJ_BOOL;
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

Ad_Function_Object::Ad_Function_Object() {
    type = OBJ_FUNCTION;
}

Ad_Function_Object::Ad_Function_Object(std::vector<Ad_AST_Node*> p, Ad_AST_Node* b, Environment* e) {
    type = OBJ_FUNCTION;
    params = p;
    body = b;
    env = e;
}

/*Ad_Function_Object::~Ad_Function_Object() { // asta nu e bun
    if (body) {
        free_Ad_AST_Node_memory(body);
    }
    for (std::vector<Ad_AST_Node*>::iterator it = params.begin() ; it != params.end(); ++it) {
        Ad_AST_Node *obj = *it;
        free_Ad_AST_Node_memory(obj);
    }
    if (env) {
        delete env;
    }
}*/

std::string Ad_Function_Object::Inspect() {
    return "aaa";
}

void Ad_Function_Object::Print() {
    std::cout << "bbb";
}

Ad_Object_Type Ad_Function_Object::Type() {
    return type;
}

Ad_Error_Object::Ad_Error_Object() {
    type = OBJ_ERROR;
}

Ad_Error_Object::Ad_Error_Object(std::string m) {
    type = OBJ_ERROR;
    message = m;
}

std::string Ad_Error_Object::Inspect() {
    std::string out;
    out = message;
    return out;
}

void Ad_Error_Object::Print() {
    std::cout << message;
}

Ad_Object_Type Ad_Error_Object::Type() {
    return type;
}

void free_Ad_Object_memory(Ad_Object* obj) {
    if (obj) {
        switch(obj->type) {
            case OBJ_NULL:
                delete ((Ad_Null_Object*)obj);
            break;
            case OBJ_INT:
                delete ((Ad_Integer_Object*)obj);
            break;
            case OBJ_BOOL:
                delete ((Ad_Boolean_Object*)obj);
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
            default:
                std::cout << "MEMORY ERROR!!! object: " << object_type_map[obj->type] << "\n";
            break;
        }
    }
}

void print_Ad_Object(Ad_Object* obj) {
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
    }
}