#include "environment.h"

Environment::Environment() {
    outer = NULL;
    bootstrap = NULL;
    isBootstrapEnvironment = false;
    isGlobalEnvironment = false;
    ref_count = 0;
}

Environment::~Environment() {
    //std::cout << "cleaning up an env\n";
    for(std::map<std::string, Ad_Object* >::const_iterator it = store.begin(); it != store.end(); ++it) {
        Ad_DECREF(it->second); // asta merge
        //std::string mesaj = "vreau sa sterg" + it->second->Inspect();
        //std::cout << mesaj << "\n";
        if (isBootstrapEnvironment) {
            Ad_Object* obj = it->second;
            if (obj->type == OBJ_CLASS) {
                Ad_Class_Object* klass_object = (Ad_Class_Object*) obj;
                klass_object->attemptASTNodesDeletion = true;
            }
        }
        free_Ad_Object_memory(it->second);
    }
    if (bootstrap) {
        delete bootstrap;
    }
}

bool Environment::Check(std::string key) {
    //std::cout << "CAUT CHEIA: " << key << "\n";
    if (store.find(key) == store.end()) {
        //if (outer && outer->store.find(key) != outer->store.end()) return true;
        if (outer && outer->Check(key)) return true;
        if (bootstrap && bootstrap->Check(key)) return true;
        return false;
    }
    return true;
}

Ad_Object* Environment::Get(std::string key) {
    if (store.find(key) == store.end() ) {
        //if (outer && outer->store.find(key) != outer->store.end()) {
        //    return outer->store[key];
        //}
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

void Environment::Set(std::string key, Ad_Object* obj) {
    if (store.find(key) != store.end()) {
        int old_ref_count = store[key]->ref_count;
        FreeObjectForKey(key);
        store[key] = obj;
        Ad_INCREF(obj); // this should be old_ref_count
        //obj->ref_count = old_ref_count; // this does not work
        return;
    }
    if (outer && outer->store.find(key) != outer->store.end()) {
        int old_ref_count = outer->store[key]->ref_count;
        outer->FreeObjectForKey(key);
        outer->store[key] = obj;
        //while (old_ref_count--) Ad_INCREF(obj); // this does not work
        Ad_INCREF(obj);
        return;
    }
    if (store.find(key) != store.end()) { // pe aici nu cred ca se mai intra
        // sterge obiectul vechi daca e o suprascriere de element
        FreeObjectForKey(key);
    }
    store[key] = obj;
    Ad_INCREF(obj);
}

void Environment::SetCallArgument(std::string key, Ad_Object* obj) {
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

void Environment::SetBootstrapEnvironment(Environment *b) {
    bootstrap = b;
}

void Environment::FreeObjectForKey(std::string key) {
    Ad_DECREF(store[key]);
    free_Ad_Object_memory(store[key]);
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
        /*size = outer->store.size();
        total = 0;
        for(std::map<std::string, Ad_Object* >::const_iterator it = outer->store.begin(); it != outer->store.end(); ++it) {
            std::cout << it->first << ": ";
            std::cout << it->second->Inspect();
            total++;
            if (total < size) std::cout << ", "; // hmmm, this needs to be fixed
        }*/
        outer->PrintStore(level + 4);
    }
    std::cout << "}\n";
}

Environment NewEnvironment() {
    Environment env;
    return env;
}

Environment* newEnvironment() {
    Environment *env = new Environment();
    return env;
}

Environment NewEnclosedEnvironment(Environment *o) {
    Environment env;
    env.SetOuterEnvironment(o);
    return env;
}

Environment* newEnclosedEnvironment(Environment *o) {
    Environment* env = new Environment();
    env->SetOuterEnvironment(o);
    return env;
}

Environment NewEnclosedEnvironment(Environment *o, Environment *b) {
    Environment env;
    env.SetOuterEnvironment(o);
    env.SetBootstrapEnvironment(b);
    return env;
}

Environment* newEnclosedEnvironmentUnfreeable(Environment *o) {
    Environment* env = new Environment();
    env->SetOuterEnvironment(o);
    return env;
}