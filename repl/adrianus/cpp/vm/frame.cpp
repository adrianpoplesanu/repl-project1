#include "frame.h"

Frame::Frame(AdClosureObject* cl, int ip, int base_pointer, AdCompiledInstance* bound_instance)
    : cl(cl), ip(ip), base_pointer(base_pointer), bound_instance(bound_instance) {
}

Instructions* Frame::instructions() {
    return cl->fn->instructions;
}
