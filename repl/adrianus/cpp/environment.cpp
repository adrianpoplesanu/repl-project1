#include "environment.h"

Environment::Environment() {

}

Environment::~Environment() {
    for(std::map<std::string, Ad_Object* >::const_iterator it = store.begin(); it != store.end(); ++it) {
        free_Ad_Object_memory(it->second);
    }
}

bool Environment::Check(std::string key) {
    if (store.find(key) == store.end() ) {
        return false;
    }
    return true;
}

Ad_Object* Environment::Get(std::string key) {
    if (store.find(key) == store.end() ) {
        //return NULL;
        if (outer.find(key) == outer.end()) {
            return NULL;
        } else {
            return outer[key];
        }
    } else {
        return store[key];
    }
}

void Environment::Set(std::string key, Ad_Object* obj) {
    if (Get(key) != NULL) {
        // sterge obiectul vechi daca e o suprascriere de element
        FreeObjectForKey(key);
    }
    store[key] = obj;
    Ad_INCREF(obj);
}

void Environment::SetOuterEnvironment(Environment o) {
    outer.clear();
    for(std::map<std::string, Ad_Object* >::const_iterator it = store.begin(); it != store.end(); ++it) {
        outer[it->first] = it->second;
    }
}

void Environment::FreeObjectForKey(std::string key) {
    Ad_DECREF(store[key]);
    free_Ad_Object_memory(store[key]);
}

void Environment::PrintStore() {
    std::cout << "store:\n";
    for(std::map<std::string, Ad_Object* >::const_iterator it = store.begin(); it != store.end(); ++it) {
        std::cout << it->first << "\n";
        print_Ad_Object(it->second);
    }
}

Environment NewEnvoronment() {
    Environment env;
    return env;
}

Environment NewEnclosedEnvironment(Environment outer) {
    Environment env;
    env.SetOuterEnvironment(outer);
    return env;
}