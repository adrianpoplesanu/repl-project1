#include "objects.h"
#include "signal.h"

void free_builtin_arguments(std::vector<Ad_Object*>);

Ad_Object* len_builtin(std::vector<Ad_Object*> args) {
    // toate builtinurile cred ca ar trebui sa primeasca o lista de argumente, ca sa fie unitate in antet
    Ad_Object* obj = args[0];
    if (obj->Type() == OBJ_STRING) {
        int length = ((Ad_String_Object*)obj)->value.length();
        free_builtin_arguments(args);
        return new Ad_Integer_Object(length);
    }
    free_builtin_arguments(args);
    return new Ad_Integer_Object(0);
}

Ad_Object* exit_builtin(std::vector<Ad_Object*> args) {
    // TODO: i need to free all args, cred ca toate builtin-urile care primesc args, ar trebui sa faca un free pe ele
    free_builtin_arguments(args);
    Ad_Signal_Object* signal = new Ad_Signal_Object();
    signal->signal_type = SIGNAL_EXIT;
    return signal;
}

Ad_Object* print_builtin(std::vector<Ad_Object*> args) {
    Ad_Object* obj = args[0];
    std::cout << obj->Inspect() << "\n";
    free_builtin_arguments(args);
    return NULL;
}

Ad_Object* ref_count(std::vector<Ad_Object*> args) {
    Ad_Object* target = args[0];
    Ad_Integer_Object* obj = new Ad_Integer_Object(target->ref_count);
    return obj;
}

Ad_Object* type_builtin(std::vector<Ad_Object*> args) {
    Ad_Object* target = args[0];
    std::string type = object_type_map[target->Type()];
    Ad_String_Object* obj = new Ad_String_Object(type);
    return obj; // TODO: check for potential memory leak
}

Ad_Object* append_builtin(std::vector<Ad_Object*> args) {
    Ad_List_Object* target = (Ad_List_Object*)args[0];
    Ad_Object* obj = args[1];
    //Ad_INCREF(obj); // in EvalExpressions din List Object evaluator asta nu e apelat, pentru ca atata timp cat nu se dezaloca ListObject nu se apeleaza free_Ad_Object_memory
    target->elements.push_back(obj);
    return NULL;
}

Ad_Object* pop_builtin(std::vector<Ad_Object*> args) {
    return NULL;
}

Ad_Object* remove_builtin(std::vector<Ad_Object*> args) {
    return NULL;
}

Ad_Object* upper_builtin(std::vector<Ad_Object*> args) {
    return NULL;
}

Ad_Object* lower_builtin(std::vector<Ad_Object*> args) {
    return NULL;
}

// TODO: Ad_Builtin_Object needs a function pointer in the constructor, which in case of len, will point to len_builtin
std::map<std::string, Ad_Object*> builtins_map = {
    {"len", new Ad_Builtin_Object(&len_builtin)},
    {"exit", new Ad_Builtin_Object(&exit_builtin)},
    {"print", new Ad_Builtin_Object(&print_builtin)},
    {"ref_count", new Ad_Builtin_Object(&ref_count)},
    {"type", new Ad_Builtin_Object(&type_builtin)},
    {"append", new Ad_Builtin_Object(&append_builtin)},
    {"pop", new Ad_Builtin_Object(&pop_builtin)},
    {"remove", new Ad_Builtin_Object(&remove_builtin)},
    {"upper", new Ad_Builtin_Object(&upper_builtin)},
    {"lower", new Ad_Builtin_Object(&lower_builtin)}
};

void free_builtin_arguments(std::vector<Ad_Object*> args) {
    for (std::vector<Ad_Object*>::iterator it = args.begin() ; it != args.end(); ++it) {
        Ad_Object *obj = *it;
        free_Ad_Object_memory(obj);
    }
}

void free_builtin_map() {
    for (std::map<std::string, Ad_Object*>::iterator it = builtins_map.begin(); it != builtins_map.end(); ++it) {
        free_Ad_Object_memory(it->second);
    }
}