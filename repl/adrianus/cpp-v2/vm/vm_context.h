#ifndef AD_VM_CONTEXT_H
#define AD_VM_CONTEXT_H

class VM;

VM* ad_current_vm();
void ad_set_current_vm(VM* vm);

#endif
