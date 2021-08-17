#ifndef __LISTOBJECT_H
#define __LISTOBJECT_H

#include "objects.h"

class Ad_List {
private:
    Ad_Object **data;
    int capacity;
    int size;
public:
    Ad_List();
    ~Ad_List();
    Ad_Object* Get(int);
    void Set(Ad_Object*, int);
    void Add(Ad_Object*, int);
    Ad_Object* Remove(int);
    int Size();
    int Capacity();
    bool IsEmpty();
    void Reverse();
    void Sort();
    void Resize(int);
    void Append(Ad_Object*);

    bool IsValidBounds(int);
};

#endif