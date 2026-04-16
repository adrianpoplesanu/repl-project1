#include "symbol_table.h"

// Symbol implementation
Symbol::Symbol() : name(""), scope(SymbolScope::GLOBAL), index(0), class_index(-1) {
}

Symbol::Symbol(const std::string& name, SymbolScope scope, int index, int class_index) 
    : name(name), scope(scope), index(index), class_index(class_index) {
}

// SymbolTable implementation
SymbolTable::SymbolTable() 
    : outer(nullptr), num_definitions(0) {
    store = std::unordered_map<std::string, Symbol>();
    free_symbols = std::vector<Symbol>();
    class_symbols = std::vector<std::string>();
}

SymbolTable::SymbolTable(SymbolTable* outer) 
    : outer(outer), num_definitions(0) {
    store = std::unordered_map<std::string, Symbol>();
    free_symbols = std::vector<Symbol>();
    class_symbols = std::vector<std::string>();
}

SymbolTable::~SymbolTable() {
    // Note: We don't delete outer here as it may be managed elsewhere
}

Symbol SymbolTable::define(const std::string& name, bool is_class_scope) {
    // TODO: am adaugat if-ul asta ca sa nu generez simboluri noi pentru acelasi identificator
    // ca sa pot sa fac i = 5 i = i + 1, a doua atribuire creeaza un simbol nou cu index 1, si i + 1 nu stii ce e
    Symbol symbol;
    if (store.find(name) != store.end()) {
        Symbol* resolved = resolve(name);
        if (resolved != nullptr) {
            symbol = *resolved;
        } else {
            symbol = Symbol(name, SymbolScope::GLOBAL, num_definitions);
        }
    } else {
        symbol = Symbol(name, SymbolScope::GLOBAL, num_definitions);
    }
    
    if (outer == nullptr) {
        symbol.scope = SymbolScope::GLOBAL;
    } else {
        if (is_class_scope) {
            symbol.scope = SymbolScope::CLASS;
        } else {
            symbol.scope = SymbolScope::LOCAL;
        }
    }
    
    store[name] = symbol;
    num_definitions++;
    return symbol;
}

Symbol* SymbolTable::resolve(const std::string& name) {
    auto it = store.find(name);
    if (it != store.end()) {
        return &(it->second);
    }
    
    if (outer != nullptr) {
        Symbol* obj = outer->resolve(name);
        if (obj != nullptr) {
            if (obj->scope == SymbolScope::GLOBAL || obj->scope == SymbolScope::BUILTIN) {
                return obj;
            }
            if (obj->scope == SymbolScope::CLASS) {
                return obj;
            }
            // For LOCAL or other scopes, define as free
            Symbol free = define_free(*obj);
            return &store[name];
        }
        // obj is None - variable not found in scope
        // TODO: is this correct? please check this
        return nullptr;
    }
    
    return nullptr;
}

Symbol SymbolTable::define_builtin(int index, const std::string& name) {
    Symbol symbol(name, SymbolScope::BUILTIN, index);
    store[name] = symbol;
    return symbol;
}

Symbol SymbolTable::define_free(const Symbol& original) {
    free_symbols.push_back(original);
    Symbol symbol(original.name, SymbolScope::FREE, static_cast<int>(free_symbols.size() - 1));
    store[original.name] = symbol;
    return symbol;
}

Symbol SymbolTable::define_function_name(const std::string& name) {
    Symbol symbol(name, SymbolScope::FUNCTION, 0);
    store[name] = symbol;
    return symbol;
}

Symbol SymbolTable::define_class_name(const std::string& name, int class_index) {
    if (name == "this") {
        if (class_symbols.size() == 0) {
            class_symbols.push_back(name);
        }
        Symbol symbol(name, SymbolScope::CLASS, 0, class_index);
        store[name] = symbol;
        return symbol;
    }
    
    class_symbols.push_back(name);
    Symbol symbol(name, SymbolScope::CLASS, static_cast<int>(class_symbols.size() - 1), class_index);
    store[name] = symbol;
    return symbol;
}

Symbol SymbolTable::define_this(const std::string& name, int class_index) {
    if (class_symbols.size() == 0) {
        class_symbols.push_back(name);
    }
    Symbol symbol(name, SymbolScope::CLASS, 0, class_index);
    store[name] = symbol;
    return symbol;
}

// Helper functions
SymbolTable* new_symbol_table() {
    return new SymbolTable();
}

SymbolTable* new_enclosed_symbol_table(SymbolTable* outer) {
    return new SymbolTable(outer);
}
