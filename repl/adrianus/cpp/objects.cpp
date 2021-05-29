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

Ad_Integer_Object::Ad_Integer_Object() {
    type = OBJ_INT;
}

Ad_Integer_Object::Ad_Integer_Object(int v) {
    type = OBJ_INT;
    value = v;
}

std::string Ad_Integer_Object::Inspect() {
    std::string out = "";
    return out;
}

void Ad_Integer_Object::Print() {
    std::cout << value;
}

Ad_Object_Type Ad_Integer_Object::Type() {
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
    return out;
}

void Ad_Boolean_Object::Print() {
    std::cout << value;
}

Ad_Object_Type Ad_Boolean_Object::Type() {
    return type;
}