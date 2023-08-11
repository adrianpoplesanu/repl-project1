#include "environment.h"

Environment::Environment() {
    outer = NULL;
    bootstrap = NULL;
    isBootstrapEnvironment = false;
    isGlobalEnvironment = false;
    isRunningImportCommand = false;
    ref_count = 0;
    isInstanceEnvironment = false;
    owningInstanceEnvironment = NULL;
}

Environment::~Environment() {
    for(std::map<std::string, Ad_Object* >::const_iterator it = store.begin(); it != store.end(); ++it) {
        // ... do nothing, no need to DECREF objects in store, garbage collector will handle them
    }
    for(std::map<std::string, Environment* >::const_iterator it = siblings.begin(); it != siblings.end(); ++it) {
        // TODO: do this proper, maybe mark the env for sweeping using the gc?
        Ad_DECREF(it->second);
        delete it->second;
    }
}

bool Environment::Check(std::string key) {
    if (store.find(key) == store.end()) {
        if (outer && outer->Check(key)) return true;
        if (bootstrap && bootstrap->Check(key)) return true;
        return false;
    }
    return true;
}

Ad_Object* Environment::Get(std::string key) {
    if (store.find(key) == store.end() ) {
        if (outer && outer->Check(key)) {
            return outer->Get(key);
        }
        if (bootstrap && bootstrap->Check(key)) {
            return bootstrap->Get(key);
        }
        return NULL;
    } else {
        return store[key];
    }
}

Ad_Object* Environment::lookupOnlyInStore(std::string key) {
    if (store.find(key) == store.end()) {
        return NULL;
    } else {
        return store[key];
    }
}

Ad_Object* Environment::lookupConstructor() {
    return lookupOnlyInStore("constructor");
}

void Environment::Set(std::string key, Ad_Object* obj) {
    if (store.find(key) != store.end()) {
        int old_ref_count = store[key]->ref_count;
        FreeObjectForKey(key);
        store[key] = obj;
        Ad_INCREF(obj); // this should be old_ref_count
        return;
    }
    if (outer && outer->Check(key)) {
        outer->Set(key, obj);
        return;
    }
    if (store.find(key) != store.end()) {
        // delete old object if this is an over write
        FreeObjectForKey(key);
    }
    store[key] = obj;
    Ad_INCREF(obj);
}

void Environment::setLocalParam(std::string key, Ad_Object* obj) {
    if (store.find(key) != store.end()) {
        // delete old object if this is an over write
        FreeObjectForKey(key);
    }
    store[key] = obj;
    Ad_INCREF(obj);
}

void Environment::addSibling(std::string key, Environment *env) {
    // TODO: determine a proper way to delete siblings
    /*if (siblings.find(key) != siblings.end()) {
        delete siblings[key];
    }*/
    siblings[key] = env;
}

Environment* Environment::getSibling(std::string key) {
    return siblings[key];
}

void Environment::SetOuterEnvironment(Environment* o) {
    outer = o;
}

void Environment::SetBootstrapEnvironment(Environment *b) {
    bootstrap = b;
}

void Environment::FreeObjectForKey(std::string key) {
    // TODO: this method does nothig now, remove this
    Ad_DECREF(store[key]);
}

void Environment::PrintStore(int level) {
    int k = 0;
    if (level != 0) std::cout << "\n";
    while(k++ < level) std::cout << " ";
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
    k = 0;
    while(k++ < level) std::cout << " ";
    std::cout << "outer: {";
    if (outer) {
        outer->PrintStore(level + 4);
    }
    std::cout << "}\n";
}

Environment* Environment::copy(GarbageCollector *gc) {
    // TODO: asta strica tot, se face un loop infinit aici
    Environment *result = new Environment();
    if (outer != NULL) {
        //result->outer = outer->copy(gc);
        result->outer = NULL;
    }
    if (result != NULL) {
        result->bootstrap = bootstrap; // asta n-ar trebui sa se schimbe
    }
    for(std::map<std::string, Ad_Object* >::const_iterator it = store.begin(); it != store.end(); ++it) {
        //result->store[it->first] = it->second->copy(gc);
    }
    for(std::map<std::string, Environment* >::const_iterator it = siblings.begin(); it != siblings.end(); ++it) {
        //result->siblings[it->first] = it->second->copy(gc);
    }
    return result;
}

Environment* newEnvironment() {
    Environment *env = new Environment();
    return env;
}

Environment* newEnclosedEnvironment(Environment *o) {
    Environment* env = new Environment();
    env->SetOuterEnvironment(o);
    return env;
}

Environment* newEnclosedEnvironmentUnfreeable(Environment *o) {
    Environment* env = new Environment();
    env->SetOuterEnvironment(o);
    return env;
}

void free_Ad_environment_memory(Environment* env) {
    if (env->ref_count > 0) return;
    delete env;
}

void Ad_INCREF(Environment* env) {
    if (env) {
        env->ref_count++;
    }
}

void Ad_DECREF(Environment* env) {
    if (env) {
        env->ref_count--;
    }
}