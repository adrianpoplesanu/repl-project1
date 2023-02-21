#include <iostream>
#include "listobject.h"

void test_simple_list_object() {
    std::cout << "running test_simple_list_object\n";
    Ad_List* list = new Ad_List();

    Ad_String_Object* buna = new Ad_String_Object("buna");
    list->Append(buna);

    Ad_String_Object* dimineata = new Ad_String_Object("dimineata");
    list->Append(dimineata);

    Ad_String_Object* bang = new Ad_String_Object("!");
    list->Append(bang);

    Ad_String_Object* dex = new Ad_String_Object("bebe dex");
    list->Append(dex);

    Ad_Integer_Object* num = new Ad_Integer_Object(55);
    list->Append(num);

    list->Print();
    std::cout << list->Size() << " " << list->Capacity() << "\n";

    Ad_Object *res = list->Remove(0);
    // TODO: figure this deallocation outside of gc
    free_Ad_Object_memory(res);

    list->Print();

    res = list->Remove(0);
    // TODO: figure this deallocation outside of gc
    free_Ad_Object_memory(res);

    list->Print();
    std::cout << list->Size() << " " << list->Capacity() << "\n";

    delete list;
}