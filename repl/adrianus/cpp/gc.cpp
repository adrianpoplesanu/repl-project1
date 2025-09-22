#include "gc.h"

GarbageCollector::GarbageCollector() {
    head = NULL;
    tail = NULL;
    mainEnv = NULL;

    cycle1 = 0;
    cycle2 = 0;
    cycle3 = 0;
}

GarbageCollector::~GarbageCollector() {
    //std::cout << "when quitting garbage collector i still have " << gc_environments.size() << " environments\n";
    forceFreeEnvironments();
}

void GarbageCollector::addEnvironment(Environment *env) {
    //std::cout << "added new environment to gc\n";
    gc_environments.push_back(env);
}

void GarbageCollector::sweepEnvironments() {
    cycle1++;
    if (cycle1 < maxCycle1) {
        return;
    }
    cycle1 = 0;
    //std::cout << "running sweepEnvironments()\n";
    int count = 0;
    std::vector<Environment*> referencedEnvironments;
    //std::cout << "trying to delete " << gc_environments.size() << " environments\n";
    for (Environment *env : gc_environments) {
        if (env->ref_count > 0) {
            //...
            referencedEnvironments.push_back(env);
        } else {
            count++;
            free_Ad_environment_memory(env);
        }
    }
    //std::cout << "freed " << count << " environments\n";
    //std::cout << "skipped " << referencedEnvironments.size() << " environments\n";
    gc_environments = referencedEnvironments;
}

void GarbageCollector::forceFreeEnvironments() {
    for (Environment *env : gc_environments) {
        env->ref_count = 0;
        free_Ad_environment_memory(env);
    }
}

void GarbageCollector::clearAstNodes() {
    gc_ast_nodes.clear();
}

void GarbageCollector::clearEnvironments() {
    gc_environments.clear();
}

void GarbageCollector::clearObjects() {
    gc_objects.clear();
}

void GarbageCollector::scheduleEnvironmentToDECREF(Environment *env) {
    scheduled_to_DECREF_environments.push_back(env);
}

void GarbageCollector::consumeScheduledDECREFEnvironments() {
    cycle3++;
    if (cycle3 < maxCycle3) {
        return;
    }
    cycle3 = 0;
    for (Environment *env : scheduled_to_DECREF_environments) {
        Ad_DECREF(env);
    }
    scheduled_to_DECREF_environments.clear();
}

void GarbageCollector::addObject(Ad_Object* obj) {
    if (obj == NULL) return;
    if (head == NULL) {
        head = tail = obj;
        obj->prev = NULL;
        obj->next = NULL;
    } else {
        tail->next = obj;
        obj->prev = tail;
        obj->next = NULL;
        tail = obj;
    }
}

void GarbageCollector::markObjects() {
    //return;
    Ad_Object* iter = head;
    while (iter != NULL) {
        iter->marked = false;
        iter = iter->next;
    }

    if (mainEnv != NULL) {
        for (const std::pair<const std::string, Ad_Object*>& it : mainEnv->store) {
            markObject(it.second);
        }
    } else {
        //std::cout << "oops!!! nu am environment principal\n";
    }
    
    // for each entry in all environment visit all the objects and mark them
    for (Environment *env : gc_environments) {
        //if (env->bootstrap != NULL) {
        //    // TODO: handle the bootstrap event
        //}
        for (const std::pair<const std::string, Ad_Object*>& it : env->store) {
            markObject(it.second);
        }

        for(std::unordered_map<std::string, Environment* >::const_iterator it = env->siblings.begin(); it != env->siblings.end(); ++it) {
            for (const std::pair<const std::string, Ad_Object*>& j : it->second->store) {
                markObject(j.second);
            }
        }
    }
}

void GarbageCollector::markObject(Ad_Object* obj) {
    if (obj->marked) return;
    switch (obj->type) {
        case OBJ_NULL: {
            obj->marked = true;
            break;
        }
        case OBJ_INT: {
            obj->marked = true;
            break;
        }
        case OBJ_FLOAT: {
            obj->marked = true;
            break;
        }
        case OBJ_BOOL: {
            obj->marked = true;
            break;
        }
        case OBJ_STRING: {
            obj->marked = true;
            break;
        }
        case OBJ_RETURN_VALUE: {
            obj->marked = true;
            markObject(((Ad_ReturnValue_Object*)obj)->value);
            break;
        }
        case OBJ_FUNCTION: {
            obj->marked = true;
            // TODO: need to figure out what to do with the body AST Node and params AST Nodes
            break;
        }
        case OBJ_ERROR: {
            obj->marked = true;
            break;
        }
        case OBJ_BUILTIN: {
            obj->marked = true;
            break;
        }
        case OBJ_SIGNAL: {
            obj->marked = true;
            break;
        }
        case OBJ_LIST: {
            obj->marked = true;
            Ad_List_Object *listObject = (Ad_List_Object*) obj;
            for (std::vector<Ad_Object*>::iterator it = listObject->elements.begin() ; it != listObject->elements.end(); ++it) {
                markObject(*it);
            }
            break;
        }
        case OBJ_HASH: {
            obj->marked = true;
            Ad_Hash_Object *hashObject = (Ad_Hash_Object*) obj;
            for(std::unordered_map<std::string, HashPair>::iterator it = hashObject->pairs.begin(); it != hashObject->pairs.end(); it++) {
                markObject(it->second.GetKey());
                markObject(it->second.GetValue());
            }
            break;
        }
        case OBJ_CLASS: {
            obj->marked = true;
            // TODO: i need to determine what to do with the ASTNodes contained in class object
            break;
        }
        case OBJ_INSTANCE: {
            obj->marked = true;
            // TODO: i need to determine what to do with the contained Environment* object
            Ad_Class_Instance *instanceObject = (Ad_Class_Instance*) obj;
            markObject(instanceObject->klass_object);
            for (const std::pair<const std::string, Ad_Object*>& it : instanceObject->instance_environment->store) {
                markObject(it.second);
            }
            break;
        }
        case OBJ_FILE: {
            obj->marked = true;
            break;
        }
        case OBJ_SOCKET: {
            obj->marked = true;
            break;
        }
        case OBJ_THREAD: {
            obj->marked = true;
            Ad_Thread_Object* threadObject = (Ad_Thread_Object*) obj;
            if (threadObject->callback != NULL) {
                markObject(threadObject->callback);
            }
            break;
        }
        case OBJ_BREAK: {
            obj->marked = true;
            break;
        }
        case OBJ_CONTINUE: {
            obj->marked = true;
            break;
        }

        default: {
            std::cout << "MEMORY ERROR!!! garbage collection inconsistency!\n";
            break;
        }
    }
}

void GarbageCollector::unmarkAllObjects() {
    Ad_Object* iter = head;
    int count = 0;
    while (iter != NULL) {
        iter->marked = false;
        iter = iter->next;
        count++;
    }
    //std::cout << "unmarking " << count << " objects\n";
}

void GarbageCollector::sweepObjects() {
    //return;
    Ad_Object* iter = head;
    while(iter != NULL) {
        Ad_Object *target = NULL;
        if (!iter->marked) {
            if (iter == head && iter == tail) {
                head = NULL;
                tail = NULL;
            } else if (iter == head) {
                head = iter->next;
                head->prev = NULL;
            } else if (iter == tail) {
                tail = tail->prev;
                tail->next = NULL;
            } else {
                Ad_Object *p = iter->prev;
                Ad_Object *n = iter->next;
                p->next = n;
                n->prev = p;
            }
            target = iter;
        }
        iter = iter->next;
        if (target) {
            // free the object
            free_Ad_Object_memory(target);
            //std::cout << target->Inspect() << "\n"; // asta merge daca nu fac free inainte cu metoda veche
        }
    }
}

int GarbageCollector::getTotalResidualGCObjects() {
    int result = 0;
    Ad_Object* iter = head;
    while(iter != NULL) {
        iter = iter->next;
        result++;
    }
    return result;
}

void GarbageCollector::forceFreeObjects() {
    unmarkAllObjects();
    sweepObjects();
}

void GarbageCollector::markObjects(Ad_Object **stack, int sp) {

}
