#include "listobject.h"

Ad_List::Ad_List() {
    capacity = 4;
    size = 0;
    data = new Ad_Object*[capacity];
}

Ad_List::~Ad_List() {
    for (int i = 0; i < size; i++) free_Ad_Object_memory(data[i]);
    delete[] data;
}