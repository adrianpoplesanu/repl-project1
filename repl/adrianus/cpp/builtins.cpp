#include "objects.h"
#include "signal.h"
#include "environment.h"
#include <stdio.h>

void free_builtin_arguments(std::vector<Ad_Object*>);

Ad_Object* len_builtin(std::vector<Ad_Object*> args, Environment* env) {
    // toate builtinurile cred ca ar trebui sa primeasca o lista de argumente, ca sa fie unitate in antet
    Ad_Object* obj = args[0];
    if (obj->Type() == OBJ_STRING) {
        Ad_String_Object* error_obj = (Ad_String_Object*) obj;
        int length = error_obj->value.length();
        free_builtin_arguments(args);
        return new Ad_Integer_Object(length);
    }
    if (obj->Type() == OBJ_LIST) {
        Ad_List_Object* list_obj = (Ad_List_Object*) obj;
        return new Ad_Integer_Object(list_obj->elements.size());
    }
    if (obj->Type() == OBJ_HASH) {
        return new Ad_Integer_Object(0);
    }
    free_builtin_arguments(args);
    return new Ad_Integer_Object(0);
}

Ad_Object* exit_builtin(std::vector<Ad_Object*> args, Environment* env) {
    // TODO: i need to free all args, cred ca toate builtin-urile care primesc args, ar trebui sa faca un free pe ele
    free_builtin_arguments(args);
    Ad_Signal_Object* signal = new Ad_Signal_Object();
    signal->signal_type = SIGNAL_EXIT;
    return signal;
}

Ad_Object* print_builtin(std::vector<Ad_Object*> args, Environment* env) {
    Ad_Object* obj = args[0];
    std::cout << obj->Inspect() << "\n";
    free_builtin_arguments(args);
    return NULL;
}

Ad_Object* ref_count(std::vector<Ad_Object*> args, Environment* env) {
    Ad_Object* target = args[0];
    Ad_Integer_Object* obj = new Ad_Integer_Object(target->ref_count);
    return obj;
}

Ad_Object* type_builtin(std::vector<Ad_Object*> args, Environment* env) {
    Ad_Object* target = args[0];
    std::string type = object_type_map[target->Type()];
    Ad_String_Object* obj = new Ad_String_Object(type);
    return obj; // TODO: check for potential memory leak
}

Ad_Object* append_builtin(std::vector<Ad_Object*> args, Environment* env) {
    Ad_List_Object* target = (Ad_List_Object*)args[0];
    Ad_Object* obj = args[1];
    //Ad_INCREF(obj); // in EvalExpressions din List Object evaluator asta nu e apelat, pentru ca atata timp cat nu se dezaloca ListObject nu se apeleaza free_Ad_Object_memory
    target->elements.push_back(obj);
    return NULL;
}

Ad_Object* pop_builtin(std::vector<Ad_Object*> args, Environment* env) {
    return NULL;
}

Ad_Object* remove_builtin(std::vector<Ad_Object*> args, Environment* env) {
    return NULL;
}

Ad_Object* upper_builtin(std::vector<Ad_Object*> args, Environment* env) {
    return NULL;
}

Ad_Object* lower_builtin(std::vector<Ad_Object*> args, Environment* env) {
    return NULL;
}

Ad_Object* context_builtin(std::vector<Ad_Object*> args, Environment* env) {
    env->PrintStore();
    return NULL;
}

Ad_Object* __iofile_builtin(std::vector<Ad_Object*> args, Environment* env) {
    Ad_String_Object* filename1 = (Ad_String_Object*) args[0];
    Ad_String_Object* _operator1 = (Ad_String_Object*) args[1];
    Ad_File_Object* obj = new Ad_File_Object(filename1->value, _operator1->value);
    return obj;
}

Ad_Object* system_builtin(std::vector<Ad_Object*> args, Environment* env) {
    Ad_String_Object* command_argument = (Ad_String_Object*) args[0];
    system(command_argument->value.c_str());
    Ad_String_Object* obj = new Ad_String_Object("bla bla bli");
    return obj;
}

Ad_Object* __syssystem_builtin(std::vector<Ad_Object*> args, Environment *env) {
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

    pclose(fpipe);
    Ad_String_Object* obj = new Ad_String_Object(result);
    return obj;
}

Ad_Object* __iosocket_builtin(std::vector<Ad_Object*> args, Environment* env) {
    return NULL;
}

Ad_Object* eval_builtin(std::vector<Ad_Object*> args, Environment* env) {
    return NULL;
}

Ad_Object* first_builtin(std::vector<Ad_Object*> args, Environment *env) {
    return NULL;
}

Ad_Object* input_builtin(std::vector<Ad_Object*> args, Environment *env) {
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
    {"lower", new Ad_Builtin_Object(&lower_builtin)},
    {"context", new Ad_Builtin_Object(&context_builtin)},
    {"__iofile", new Ad_Builtin_Object(&__iofile_builtin)},
    {"__syssystem", new Ad_Builtin_Object(&__syssystem_builtin)},
    {"__iosocket", new Ad_Builtin_Object(&__iosocket_builtin)},
    {"eval", new Ad_Builtin_Object(&eval_builtin)},
    {"first", new Ad_Builtin_Object(&first_builtin)},
    {"input", new Ad_Builtin_Object(&input_builtin)}
    // eval
    // first
    // input
    // https://www.w3schools.com/python/python_ref_keywords.asp
	// https://www.w3schools.com/python/python_ref_functions.asp
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
