#include "instructions.h"

unsigned char Instructions::get(int i) {
    return bytes.at(i);
}

void Instructions::add(unsigned char val) {
    bytes.push_back(val);
}

void Instructions::removeLast() {
    bytes.pop_back();
    size--;
}