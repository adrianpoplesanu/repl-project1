#include "listobject2.h"

template<typename T>
InternalAdList<T>::InternalAdList() {
    elements = new T[DEFAULT_CAPACITY];
    size = 0;
    capacity = DEFAULT_CAPACITY;
}

template<typename T>
void InternalAdList<T>::add(T e) {
    if (size >= capacity) {
        capacity *= FACTOR;
        T* newElements = new T[capacity];
        for (int i = 0; i < size; i++) {
            newElements[i] = elements[i];
        }
        T* old = elements;
        elements = newElements;
        delete old;
    }
    elements[size++] = e;
}

template<typename T>
T InternalAdList<T>::get(int i) {
    return elements[i];
}

template<typename T>
void InternalAdList<T>::clear() {
    capacity = DEFAULT_CAPACITY;
    size = 0;
    T* old = elements;
    elements = new T[DEFAULT_CAPACITY];
    delete old;
}