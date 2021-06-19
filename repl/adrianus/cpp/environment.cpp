#include "environment.h"

Environment::Environment() {

}

Environment::~Environment() {
    //... i need to parse throuh all objects from store and delete them
    for(std::map<std::string, Ad_Object* >::const_iterator it = store.begin(); it != store.end(); ++it) {
        free_Ad_Object_memory(it->second);
    }
}

Ad_Object* Environment::Get(std::string key) {
    if (store.find(key) == store.end() ) {
        return NULL;
    } else {
        return store[key];
    }
}

void Environment::Set(std::string key, Ad_Object* obj) {
    store[key] = obj;
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
    // TODO this
    Environment env;
    return env;
}