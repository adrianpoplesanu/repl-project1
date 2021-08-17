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

Ad_Object* Ad_List::Get(int i) {
    if (!IsValidBounds(i)) return NULL;
    return data[i];
}

void Ad_List::Set(Ad_Object* obj, int i) {
    if (IsValidBounds(i)) {
        data[i] = obj;
    }
}

void Ad_List::Add(Ad_Object* obj, int i) {

}

Ad_Object* Ad_List::Remove(int i) {
    return NULL;
}

int Ad_List::Size() {
    return size;
}

bool Ad_List::IsEmpty() {
    return size == 0;
}

void Ad_List::Reverse() {

}

void Ad_List::Sort() {

}

void Ad_List::Resize(int n) {

}

void Ad_List::Append(Ad_Object* obj) {

}

bool Ad_List::IsValidBounds(int i) {
    if (i < 0 || i > size) {
        return false;
    }
    return true;
}