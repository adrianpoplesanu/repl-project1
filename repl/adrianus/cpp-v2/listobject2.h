#ifndef __LISTOBJECT2_H
#define __LISTOBJECT2_H

template<typename T>
class InternalAdList {
public:
    T* elements;
    int size;
    int capacity;
    int DEFAULT_CAPACITY = 4;
    int FACTOR = 2;

    InternalAdList();
    void add(T);
    T get(int);
    void clear();
};

#endif