#include "objects.h"

Ad_Object* len_builtin(std::vector<Ad_Object*> args) {
    // toate builtinurile cred ca ar trebui sa primeasca o lista de argumente, ca sa fie unitate in antet
    Ad_Object* obj = args[0];
    if (obj->Type() == OBJ_STRING) {
        int length = ((Ad_String_Object*)obj)->value.length();
        return new Ad_Integer_Object(length);
    }
    return new Ad_Integer_Object(0);
}

Ad_Object* exit_builtin(std::vector<Ad_Object*> args) { // this doesn't work yet
    return NULL;
}

Ad_Object* print_builtin(std::vector<Ad_Object*> args) {
    return NULL;
}

// TODO: Ad_Builtin_Object needs a function pointer in the constructor, which in case of len, will point to len_builtin
std::map<std::string, Ad_Object*> builtins_map = {
    {"len", new Ad_Builtin_Object(&len_builtin)},
    {"exit", new Ad_Builtin_Object(&exit_builtin)},
    {"print", new Ad_Builtin_Object(&print_builtin)}
};

void free_builtin_map() {
    for (std::map<std::string, Ad_Object*>::iterator it = builtins_map.begin(); it != builtins_map.end(); ++it) {
        delete it->second;
    }
}