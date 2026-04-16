#include "instructions.h"

Instructions::Instructions() {
    size = 0;
}

unsigned char Instructions::get(int i) {
    return bytes.at(i);
}

void Instructions::add(unsigned char val) {
    bytes.push_back(val);
    size++;
}

void Instructions::removeLast() {
    bytes.pop_back();
    size--;
}