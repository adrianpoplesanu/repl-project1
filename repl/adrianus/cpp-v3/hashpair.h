#ifndef __HASHPAIR_H
#define __HASHPAIR_H

class Ad_Object; // forward declaration

class HashPair {
public:
    Ad_Object *key;
    Ad_Object *value;

    HashPair();
    HashPair(Ad_Object*, Ad_Object*);
    ~HashPair();

    Ad_Object* GetKey();
    Ad_Object* GetValue();
};

#endif