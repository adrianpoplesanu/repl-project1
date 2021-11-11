#include "environment.h"

Environment::Environment() {
    outer = NULL;
}

Environment::~Environment() {
    for(std::map<std::string, Ad_Object* >::const_iterator it = store.begin(); it != store.end(); ++it) {
        Ad_DECREF(it->second); // asta merge
        free_Ad_Object_memory(it->second);
    }
}

bool Environment::Check(std::string key) {
    if (store.find(key) == store.end()) {
        if (outer && outer->store.find(key) != outer->store.end()) return true;
        return false;
    }
    return true;
}

Ad_Object* Environment::Get(std::string key) {
    if (store.find(key) == store.end() ) {
        if (outer && outer->store.find(key) != outer->store.end()) {
            return outer->store[key];
        }
        return NULL;
    } else {
        return store[key];
    }
}

void Environment::Set(std::string key, Ad_Object* obj) {
    if (store.find(key) != store.end()) {
        // sterge obiectul vechi daca e o suprascriere de element
        FreeObjectForKey(key);
    }
    store[key] = obj;
    Ad_INCREF(obj);
}

void Environment::SetOuterEnvironment(Environment* o) {
    outer = o;
}

void Environment::FreeObjectForKey(std::string key) {
    Ad_DECREF(store[key]);
    free_Ad_Object_memory(store[key]);
}

void Environment::PrintStore() {
    std::cout << "store: {";
    int size = store.size();
    int total = 0;
    for(std::map<std::string, Ad_Object* >::const_iterator it = store.begin(); it != store.end(); ++it) {
        std::cout << it->first << ": ";
        std::cout << it->second->Inspect();
        total++;
        if (total < size) std::cout << ", "; // hmmm, this needs to be fixed
    }
    std::cout << "}\n";
}

Environment NewEnvironment() {
    Environment env;
    return env;
}

Environment NewEnclosedEnvironment(Environment *o) {
    Environment env;
    env.SetOuterEnvironment(o);
    return env;
}