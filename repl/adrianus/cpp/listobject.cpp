#include "listobject.h"

Ad_List::Ad_List() {
    capacity = 4;
    size = 0;
    data = new Ad_Object*[capacity];
}

Ad_List::~Ad_List() {
    // TODO: figure this deallocation outside gc
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
    if (!IsValidBounds(i)) {
        std::cout << "Out of bounds, i should return an Ad Error Object\n";
        return;
    }
    if (size == capacity) {
        Resize(capacity * 2);
    }
    for (int j = size - 1; j > i; j--) data[j] = data[j - 1];
    data[i] = obj;
    size++;
}

Ad_Object* Ad_List::Remove(int i) {
    if (!IsValidBounds(i)) {
        std::cout << "Out of bounds, i should return an Ad Error Object\n";
        return NULL;
    }
    Ad_Object* obj = data[i];
    for (int j = i; j < size - 1; j++) data[j] = data[j + 1];
    size--;
    if (size < capacity / 2 && capacity > 4) {
        Resize(capacity / 2);
    }
    return obj;
}

int Ad_List::Size() {
    return size;
}

int Ad_List::Capacity() {
    return capacity;
}

bool Ad_List::IsEmpty() {
    return size == 0;
}

void Ad_List::Reverse() {

}

void Ad_List::Sort() {

}

void Ad_List::Resize(int n) {
    Ad_Object** current = data;
    data = new Ad_Object*[n];
    for (int i = 0; i < size; i++) data[i] = current[i];
    capacity = n;
    delete[] current;
}

void Ad_List::Append(Ad_Object* obj) {
    Add(obj, size);
}

bool Ad_List::IsValidBounds(int i) {
    if (i < 0 || i > size) {
        return false;
    }
    return true;
}

std::string Ad_List::Inspect() {
    std::string out = "[";
    for (int i = 0; i < size; i++) {
        out += data[i]->Inspect();
        if (i < size - 1) out += ", ";
    }
    out += "]";
    return out;
}

void Ad_List::Print() {
    std::string out = "[";
    for (int i = 0; i < size; i++) {
        out += data[i]->Inspect();
        if (i < size - 1) out += ", ";
    }
    out += "]";
    std::cout << out << "\n";
}