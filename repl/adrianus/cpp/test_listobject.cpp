#include <iostream>
#include "listobject.h"

void test_simple_list_object() {
    std::cout << "running test_simple_list_object\n";
    Ad_List* list = new Ad_List();

    Ad_String_Object* name = new Ad_String_Object("buna dimineata!");
    list->Append(name);

    delete list;
}