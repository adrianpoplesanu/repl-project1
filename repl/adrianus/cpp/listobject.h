#ifndef __LISTOBJECT_H
#define __LISTOBJECT_H

#include "objects.h"

class Ad_List {
public:
    Ad_Object **data;
    int capacity;
    int size;

    Ad_List();
    ~Ad_List();
    Ad_Object* Get(int i);
    void Set(Ad_Object*, int i);
    void Add(Ad_Object*, int i);
    void Remove(int i);
    int Size();
    bool IsEmpty();
    void Reverse();
    void Sort();
};

#endif