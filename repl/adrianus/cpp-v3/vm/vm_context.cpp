#include "vm_context.h"
#include "vm.h"

namespace {
VM* current_vm = nullptr;
thread_local VM* tls_current_vm = nullptr;
} // namespace

VM* ad_current_vm() {
    if (tls_current_vm != nullptr) {
        return tls_current_vm;
    }
    return current_vm;
}

void ad_set_current_vm(VM* vm) {
    current_vm = vm;
    tls_current_vm = vm;
}

void ad_set_tls_current_vm(VM* vm) {
    tls_current_vm = vm;
}
