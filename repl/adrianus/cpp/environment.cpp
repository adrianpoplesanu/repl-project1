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
    for(std::unordered_map<std::string, Environment* >::const_iterator it = siblings.begin(); it != siblings.end(); ++it) {
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
        store[key] = obj;
        return;
    }
    if (outer && outer->Check(key)) {
        outer->Set(key, obj);
        return;
    }
    store[key] = obj;
}

void Environment::setLocalParam(std::string key, Ad_Object* obj) {
    store[key] = obj;
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

void Environment::PrintStore(int level) {
    int k = 0;
    if (level != 0) std::cout << "\n";
    while(k++ < level) std::cout << " ";
    std::cout << "store: {";
    int size = store.size();
    int total = 0;
    for (const std::pair<const std::string, Ad_Object*>& it : store) {
        std::cout << it.first << ": ";
        std::cout << it.second->Inspect();
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

Ad_Object* Environment::storeToHashObject(GarbageCollector *gc) {
    std::unordered_map<std::string, HashPair> pairs;

    for (const std::pair<const std::string, Ad_Object*>& it : store) {
        std::hash<std::string> hash_string;

        Ad_Object *key = new Ad_String_Object(it.first);
        gc->addObject(key);
        Ad_Object *value = it.second;

        HashPair hash_pair(key, value);
        pairs.insert(std::make_pair(std::to_string(hash_string(key->Hash())), hash_pair)); // value needs to be a HashPair
    }

    Ad_Hash_Object *hashObject = new Ad_Hash_Object(pairs);
    gc->addObject(hashObject);
    return hashObject;
}

Ad_Object* Environment::contextToHashObject(GarbageCollector *gc) {
    std::unordered_map<std::string, HashPair> pairs;

    Ad_Hash_Object *hashObject = new Ad_Hash_Object(pairs);
    gc->addObject(hashObject);
    return hashObject;
}

std::vector<std::string> Environment::populateGetattrs() {
    std::vector<std::string> elements;
    for (const std::pair<const std::string, Ad_Object*>& it : store) {
        elements.push_back(it.first);
    }
    return elements;
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