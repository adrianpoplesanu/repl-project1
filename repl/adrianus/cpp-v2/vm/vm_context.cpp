#include "vm_context.h"
#include "vm.h"

namespace {
VM* current_vm = nullptr;
} // namespace

VM* ad_current_vm() {
    return current_vm;
}

void ad_set_current_vm(VM* vm) {
    current_vm = vm;
}
