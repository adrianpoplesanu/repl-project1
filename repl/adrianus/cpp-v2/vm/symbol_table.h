#ifndef AD_VM_SYMBOL_TABLE_H
#define AD_VM_SYMBOL_TABLE_H

#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

enum class SymbolScope {
    GLOBAL,
    LOCAL,
    BUILTIN,
    FREE,
    FUNCTION,
    CLASS
};

class Symbol {
public:
    std::string name;
    SymbolScope scope;
    int index;
    int class_index;

    Symbol();
    Symbol(const std::string& name, SymbolScope scope, int index, int class_index = -1);
};

class SymbolTable {
public:
    SymbolTable* outer;
    std::unordered_map<std::string, Symbol> store;
    std::vector<Symbol> free_symbols;
    std::vector<std::string> class_symbols;
    int num_definitions;

    SymbolTable();
    SymbolTable(SymbolTable* outer);
    ~SymbolTable();

    Symbol define(const std::string& name, bool is_class_scope = false);
    Symbol* resolve(const std::string& name);
    Symbol define_builtin(int index, const std::string& name);
    Symbol define_free(const Symbol& original);
    Symbol define_function_name(const std::string& name);
    Symbol define_class_name(const std::string& name, int class_index);
    Symbol define_this(const std::string& name, int class_index);
};

SymbolTable* new_symbol_table();
SymbolTable* new_enclosed_symbol_table(SymbolTable* outer);

#endif
