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

// TODO: Ad_Builtin_Object needs a function pointer in the constructor, which in case of len, will point to len_builtin
std::map<std::string, Ad_Object*> builtins_map = {
    {"len", new Ad_Builtin_Object(&len_builtin)},
    {"exit", new Ad_Builtin_Object(&exit_builtin)},
    {"print", new Ad_Builtin_Object(&print_builtin)}
};

void free_builtin_arguments(std::vector<Ad_Object*> args) {
    for (std::vector<Ad_Object*>::iterator it = args.begin() ; it != args.end(); ++it) {
        Ad_Object *obj = *it;
        free_Ad_Object_memory(obj);
    }
}

void free_builtin_map() {
    for (std::map<std::string, Ad_Object*>::iterator it = builtins_map.begin(); it != builtins_map.end(); ++it) {
        delete it->second;
    }
}