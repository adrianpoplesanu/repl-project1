#include "objects.h"
#include "signal.h"
#include "environment.h"
#include <stdio.h>
#include "gc.h"
#include "eval_utils.h"
#include "thread_utils.h"

void free_builtin_arguments(std::vector<Ad_Object*>);

//Ad_Null_Object NULLOBJECT;

Ad_Object* len_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    // toate builtinurile cred ca ar trebui sa primeasca o lista de argumente, ca sa fie unitate in antet
    Ad_Object* obj = args[0];
    if (obj->Type() == OBJ_STRING) {
        Ad_String_Object* string_obj = (Ad_String_Object*) obj;
        int length = string_obj->value.length();
        free_builtin_arguments(args);
        Ad_Object* result = new Ad_Integer_Object(length);
        gc->addObject(result);
        return result;
    }
    if (obj->Type() == OBJ_LIST) {
        Ad_List_Object* list_obj = (Ad_List_Object*) obj;
        int length = list_obj->elements.size();
        free_builtin_arguments(args);
        Ad_Object* result = new Ad_Integer_Object(length);
        gc->addObject(result);
        return result;
    }
    if (obj->Type() == OBJ_HASH) {
        free_builtin_arguments(args);
        Ad_Object* result = new Ad_Integer_Object(0);
        gc->addObject(result);
        return result;
    }
    free_builtin_arguments(args);
    Ad_Object* result = new Ad_Integer_Object(0);
    gc->addObject(result);
    return result;
}

Ad_Object* exit_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    // TODO: i need to free all args, cred ca toate builtin-urile care primesc args, ar trebui sa faca un free pe ele
    free_builtin_arguments(args);
    Ad_Signal_Object* signal = new Ad_Signal_Object();
    gc->addObject(signal);
    signal->signal_type = SIGNAL_EXIT;
    return signal;
}

Ad_Object* print_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    Ad_Object* obj = args[0];
    if (obj == NULL) {
        // this should print null, but maybe the upstream should return an Ad_Null_Objject
        free_builtin_arguments(args);
        return NULL;
    }
    // std::cout << obj->Inspect() << "\n"; // old print builtin
    std::cout << obj->Inspect();
    free_builtin_arguments(args);
    return NULL;
}

Ad_Object* println_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    Ad_Object* obj = args[0];
    if (obj == NULL) {
        // this should print null, but maybe the upstream should return an Ad_Null_Objject
        free_builtin_arguments(args);
        return NULL;
    }
    std::cout << obj->Inspect() << "\n";
    free_builtin_arguments(args);
    return NULL;
}

Ad_Object* ref_count(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    Ad_Object* target = args[0];
    Ad_Integer_Object* obj = new Ad_Integer_Object(target->ref_count);
    gc->addObject(obj);
    return obj;
}

Ad_Object* type_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    Ad_Object* target = args[0];
    std::string type;
    switch (target->Type()) {
        case OBJ_BOOL:
            type = "BOOLEAN";
        break;
        case OBJ_INT:
            type = "INT";
        break;
        case OBJ_FLOAT:
            type = "FLOAT";
        break;
        case OBJ_STRING:
            type = "STRING";
        break;
        case OBJ_FUNCTION:
            type = "FUNCTION";
        break;
        case OBJ_LIST:
            type = "LIST";
        break;
        case OBJ_HASH:
            type = "HASH";
        break;
        case OBJ_BUILTIN:
            type = "BUILTIN";
        break;
        case OBJ_CLASS:
            type = "CLASS";
        break;
        case OBJ_INSTANCE:
            type = "INSTANCE";
        break;
        case OBJ_FILE:
            type = "FILE";
        break;
        case OBJ_SOCKET:
            type = "SOCKET";
        break;
        case OBJ_NULL:
            type = "NULL";
        break;
        default:
            type = "UNKNOWN_TYPE";
        break;
    }
    Ad_String_Object* obj = new Ad_String_Object(type);
    gc->addObject(obj);
    return obj; // TODO: check for potential memory leak
}

Ad_Object* append_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    Ad_List_Object* target = (Ad_List_Object*)args[0];
    Ad_Object* obj = args[1];
    //Ad_INCREF(obj); // in EvalExpressions din List Object evaluator asta nu e apelat, pentru ca atata timp cat nu se dezaloca ListObject nu se apeleaza free_Ad_Object_memory
    target->elements.push_back(obj);
    return NULL;
}

Ad_Object* pop_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    return NULL;
}

Ad_Object* remove_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    return NULL;
}

Ad_Object* upper_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    return NULL;
}

Ad_Object* lower_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    return NULL;
}

Ad_Object* context_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    return env->contextToHashObject(gc);
}

Ad_Object* locals_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    return env->storeToHashObject(gc);
}

Ad_Object* __iofile_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    Ad_String_Object* filename1 = (Ad_String_Object*) args[0];
    Ad_String_Object* _operator1 = (Ad_String_Object*) args[1];
    Ad_File_Object* obj = new Ad_File_Object(filename1->value, _operator1->value);
    gc->addObject(obj);
    return obj;
}

Ad_Object* system_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    Ad_String_Object* command_argument = (Ad_String_Object*) args[0];
    system(command_argument->value.c_str());
    Ad_String_Object* obj = new Ad_String_Object("bla bla bli");
    gc->addObject(obj);
    return obj;
}

Ad_Object* __syssystem_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    Ad_String_Object* command_argument = (Ad_String_Object*) args[0];
    std::string result = "";

    FILE *fpipe;
    char *command = (char*) command_argument->value.c_str();
    char c = 0;

    if (0 == (fpipe = (FILE*)popen(command, "r"))) {
        perror("popen() failed.");
        exit(EXIT_FAILURE);
    }

    while (fread(&c, sizeof c, 1, fpipe)) {
        //printf("%c", c);
        result += c;
    }

    // TODO: determine if this really is a good idea, it looks good, but i'm not 100%
    if (!result.empty()) {
        result.pop_back();
    }

    pclose(fpipe);
    Ad_String_Object* obj = new Ad_String_Object(result);
    gc->addObject(obj);
    return obj;
}

Ad_Object* __iosocket_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    Ad_Socket_Object* socketObject = new Ad_Socket_Object();
    gc->addObject(socketObject);
    Ad_String_Object* nameObject = (Ad_String_Object*) args[0];
    Ad_Integer_Object* portObject = (Ad_Integer_Object*) args[1];
    Ad_Boolean_Object* isActiveObject = (Ad_Boolean_Object*) args[2];
    Ad_Boolean_Object* isForeverObject = (Ad_Boolean_Object*) args[3];
    Ad_Boolean_Object* isClientObject = (Ad_Boolean_Object*) args[4];
    Ad_Boolean_Object* isServerObject = (Ad_Boolean_Object*) args[5];

    socketObject->name = nameObject->value;
    socketObject->port = portObject->value;
    socketObject->isActive = isActiveObject->value;
    socketObject->isForever = isForeverObject->value;
    socketObject->isClient = isClientObject->value;
    socketObject->isServer = isServerObject->value;
    return socketObject;
}

Ad_Object* __memory_address_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    Ad_Object *target = args[0];
    std::stringstream ss;
    ss << std::hex << target;
    Ad_String_Object *result = new Ad_String_Object(ss.str());
    gc->addObject(result);
    return result;
}

Ad_Object* eval_builtin(std::vector<Ad_Object*> args, Environment* env, GarbageCollector *gc) {
    Ad_String_Object* unescapedSource = (Ad_String_Object*) args[0];
    evalSource(unescapedSource->value, env, gc);
    return NULL;
}

Ad_Object* first_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    return NULL;
}

Ad_Object* last_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    return NULL;
}

Ad_Object* map_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    return NULL;
}

Ad_Object* input_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    // TODO: add params to input function for printing something before inputting, if no params just silently wait
    std::string line;
    std::getline(std::cin, line);
    Ad_String_Object* obj = new Ad_String_Object(line);
    gc->addObject(obj);
    return obj;
}

Ad_Object* list_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    //std::cout << args.size() << "\n";
    if (args.size() == 0) {
        Ad_Object* result = new Ad_List_Object();
        gc->addObject(result);
        return result;
    }
    if (args.size() == 1) {
        Ad_List_Object* list_object = new Ad_List_Object();
        gc->addObject(list_object);
        int size = ((Ad_Integer_Object*) args[0])->value;
        //std::cout << size << "\n";
        for (int i = 0; i < size; i++) {
            list_object->elements.push_back(&NULLOBJECT);
        }
        free_builtin_arguments(args);
        return list_object;
    }
    if (args.size() == 2) {
        Ad_List_Object* list_object = new Ad_List_Object();
        gc->addObject(list_object);
        int size = ((Ad_Integer_Object*) args[0])->value;
        Ad_Object* default_object = args[1];
        for (int i = 0; i < size; i++) {
            Ad_Object* new_object = default_object->copy(gc);
            list_object->elements.push_back(new_object);
        }
        free_builtin_arguments(args);
        return list_object;
    }
    return &NULLOBJECT;
}

Ad_Object* mat_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    // no arg: [[]]
    // one arg: [[], [], ... , []]
    // two arg: [[null, ... null], ... , [null, ... null]]
    // three arg: [[a, ... a], ... , [a, ... a]]
    return &NULLOBJECT;
}

Ad_Object* hash_builtin(std::vector<Ad_Object*> args, Environment *en, GarbageCollector *gc) {
    return NULL;
}

Ad_Object* hasattr_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    return NULL;
}

Ad_Object* getattr_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    if (args.size() == 2) {
        Ad_Object* target = args.at(0);
        std::string attrName = ((Ad_String_Object*) args.at(1))->value;

        if (target->type == OBJ_INSTANCE) {
            Ad_Class_Instance* klass_instance = (Ad_Class_Instance*) target;
            return klass_instance->instance_environment->lookupOnlyInStore(attrName);
        }

        if (target->type == OBJ_CLASS) {
            Ad_Class_Object* klass_object = (Ad_Class_Object*) target;
            // TODO: figure out what to do with this
            // should i eval the astnode and return the object as an unbounded var?
        }
    }
    return NULL;
}

Ad_Object* setattr_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    return NULL;
}

Ad_Object* getattrs_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    if (args.size() == 1) {
        Ad_Object* target = args.at(0);
        std::vector<Ad_Object*> elements;
        if (target->type == OBJ_INSTANCE) {
            Ad_Class_Instance* klass_instance = (Ad_Class_Instance*) target;
            std::vector<std::string> keys = klass_instance->instance_environment->populateGetattrs();
            for (int i = 0; i < keys.size(); i++) {
                Ad_String_Object *attrName = new Ad_String_Object(keys.at(i));
                gc->addObject(attrName);
                elements.push_back(attrName);
            }

            Ad_List_Object* result = new Ad_List_Object(elements);
            gc->addObject(result);
            return result;
        }
        if (target->type == OBJ_CLASS) {
            Ad_Class_Object* klass_object = (Ad_Class_Object*) target;
            for (int i = 0; i < klass_object->attributes.size(); i++) {
                Ad_AST_ExpressionStatement* stmt = (Ad_AST_ExpressionStatement*) klass_object->attributes.at(i);
                Ad_AST_AssignStatement* assignStmt = (Ad_AST_AssignStatement*) stmt->expression;
                Ad_String_Object *attrName = new Ad_String_Object(assignStmt->name->TokenLiteral());
                gc->addObject(attrName);
                elements.push_back(attrName);
            }

            for (int i = 0; i < klass_object->methods.size(); i++) {
                Ad_AST_AssignStatement* stmt = (Ad_AST_AssignStatement*) klass_object->methods.at(i);

                Ad_String_Object *attrName = new Ad_String_Object(stmt->name->TokenLiteral());
                gc->addObject(attrName);
                elements.push_back(attrName);
            }

            Ad_List_Object* result = new Ad_List_Object(elements);
            gc->addObject(result);
            return result; 
        }
        Ad_Error_Object* result = new Ad_Error_Object("getattrs can only be called on a class instance");
        gc->addObject(result);
        return result;
    }
    return NULL;
}

Ad_Object* thread_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    Ad_Thread_Object* threadObject = new Ad_Thread_Object();
    gc->addObject(threadObject);

    return threadObject;
}

Ad_Object* import_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    Ad_String_Object* path = (Ad_String_Object*) args[0];
    importSource(path->value, env, gc);
    return NULL;
}

Ad_Object* sleep_builtin(std::vector<Ad_Object*> args, Environment *env, GarbageCollector *gc) {
    sleep_builtin_executor(((Ad_Integer_Object*) args[0])->value);
    return NULL;
}

// TODO: Ad_Builtin_Object needs a function pointer in the constructor, which in case of len, will point to len_builtin
std::unordered_map<std::string, Ad_Object*> builtins_map = {
    {"len", new Ad_Builtin_Object(&len_builtin)},
    {"exit", new Ad_Builtin_Object(&exit_builtin)},
    {"print", new Ad_Builtin_Object(&print_builtin)},
    {"println", new Ad_Builtin_Object(&println_builtin)},
    {"ref_count", new Ad_Builtin_Object(&ref_count)},
    {"type", new Ad_Builtin_Object(&type_builtin)},
    {"__append", new Ad_Builtin_Object(&append_builtin)},
    {"__pop", new Ad_Builtin_Object(&pop_builtin)},
    {"__remove", new Ad_Builtin_Object(&remove_builtin)},
    {"__upper", new Ad_Builtin_Object(&upper_builtin)},
    {"__lower", new Ad_Builtin_Object(&lower_builtin)},
    {"__context", new Ad_Builtin_Object(&context_builtin, {0})},
    {"__locals", new Ad_Builtin_Object(&locals_builtin, {0})},
    {"__iofile", new Ad_Builtin_Object(&__iofile_builtin)},
    {"__syssystem", new Ad_Builtin_Object(&__syssystem_builtin)},
    {"__iosocket", new Ad_Builtin_Object(&__iosocket_builtin)},
    {"__memory_address", new Ad_Builtin_Object(&__memory_address_builtin)},
    {"eval", new Ad_Builtin_Object(&eval_builtin)},
    {"__first", new Ad_Builtin_Object(&first_builtin)},
    {"__last", new Ad_Builtin_Object(&last_builtin)},
    {"map", new Ad_Builtin_Object(&map_builtin)},
    {"input", new Ad_Builtin_Object(&input_builtin)},
    {"list", new Ad_Builtin_Object(&list_builtin)},
    {"mat", new Ad_Builtin_Object(&mat_builtin)},
    {"hash", new Ad_Builtin_Object(&hash_builtin)},
    {"hasattr", new Ad_Builtin_Object(&hasattr_builtin)},
    {"getattr", new Ad_Builtin_Object(&getattr_builtin)},
    {"setattr", new Ad_Builtin_Object(&setattr_builtin)},
    {"getattrs", new Ad_Builtin_Object(&getattrs_builtin)},
    {"__thread", new Ad_Builtin_Object(&thread_builtin)},
    {"import", new Ad_Builtin_Object(&import_builtin)},
    {"sleep", new Ad_Builtin_Object(&sleep_builtin)}
    // eval
    // first
    // input
    // https://www.w3schools.com/python/python_ref_keywords.asp
	// https://www.w3schools.com/python/python_ref_functions.asp
};

void free_builtin_arguments(std::vector<Ad_Object*> args) {
    // TODO: this method does nothing after gc implementation, remove this
    for (std::vector<Ad_Object*>::iterator it = args.begin() ; it != args.end(); ++it) {
        Ad_Object *obj = *it;
    }
}

void free_builtin_map() {
    for (std::unordered_map<std::string, Ad_Object*>::iterator it = builtins_map.begin(); it != builtins_map.end(); ++it) {
        // TODO: mark and sweep cleanup
        free_Ad_Object_memory(it->second); // i need to find a way to do this with gc
    }
}
