#ifndef AD_BUILTINS_REGISTRY_H
#define AD_BUILTINS_REGISTRY_H

class SymbolTable;
class Ad_Object;

/// Registers builtins on the root VM compiler symbol table (indices match OP_GET_BUILTIN).
void vm_register_builtin_symbols(SymbolTable* symbol_table);

/// Object installed at index by vm_register_builtin_symbols (same order as Python VMPrototype1 pattern).
Ad_Object* vm_get_builtin_object(int index);

#endif
