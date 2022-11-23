#include "evaluator.h"
#include "utils.h"
#include "hashpair.cpp" // nu-mi place importul asta, as fi preferat sa import doar headerul
#include "builtins.cpp"
#include "gc.cpp"


//Ad_Null_Object NULLOBJECT;
Ad_Boolean_Object TRUE(true);
Ad_Boolean_Object FALSE(false);

Ad_Object* Evaluator::Eval(Ad_AST_Node* node, Environment &env) {
    switch(node->type) {
        case ST_PROGRAM:
            return EvalProgram(node, env);
        break;
        case ST_LET_STATEMENT: {
            Ad_Object* obj = Eval(((Ad_AST_LetStatement*)node)->value, env);
            env.Set(((Ad_AST_LetStatement*)node)->name.value, obj);
            return NULL;
        }
        break;
        case ST_ASSIGN_STATEMENT: {
            return EvalAssignStatement(node, env);
        }
        break;
        case ST_RETURN_STATEMENT: {
            Ad_Object* val = Eval(((Ad_AST_ReturnStatement*)node)->value, env);
            Ad_ReturnValue_Object* obj = new Ad_ReturnValue_Object();
            obj->value = val;
            return obj;
        }
        break;
        case ST_EXPRESSION_STATEMENT:
            if (((Ad_AST_ExpressionStatement*)node)->expression) {
                return Eval(((Ad_AST_ExpressionStatement*)node)->expression, env);
            }
        break;
        case ST_IDENTIFIER: {
            return EvalIdentifier(node, env);
        }
        break;
        case ST_INTEGER: {
            Ad_Integer_Object* obj = new Ad_Integer_Object();
            obj->value = ((Ad_AST_Integer*)node)->value;
            return obj;
        }
        break;
        case ST_FLOAT:
            return EvalFloatExpression(node, env);
        break;
        case ST_BOOLEAN: {
            return NativeBoolToBooleanObject(((Ad_AST_Boolean*)node)->value);
        }
        break;
        case ST_INFIX_EXPRESSION: {
            Ad_Object* left = Eval(((Ad_AST_InfixExpression*)node)->left, env);
            Ad_Object* right = Eval(((Ad_AST_InfixExpression*)node)->right, env);
            Ad_Object* result = EvalInfixExpression(((Ad_AST_InfixExpression*)node)->_operator, left, right);
            free_Ad_Object_memory(right);
            free_Ad_Object_memory(left);
            return result;
        }
        break;
        case ST_PREFIX_EXPRESSION: {
            Ad_Object* right = Eval(((Ad_AST_PrefixExpression*)node)->right, env);
            return EvalPrefixExpression(((Ad_AST_PrefixExpression*)node)->_operator, right);
        }
        break;
        case ST_IF_EXPRESSION: {
            return EvalIfExpression(node, env);
        }
        break;
        case ST_BLOCK_STATEMENT: {
            Ad_Object* result = EvalBlockStatement(node, env);
            //GarbageCollectEnvironments(); // after a block statement eval is ended it's NOT safe to garbage collect the environment, a nested while/if would trigger an unwanted garbage collect
            return result;
        }
        break;
        case ST_FUNCTION_LITERAL: {
            Ad_Function_Object* obj = new Ad_Function_Object(((Ad_AST_FunctionLiteral*)node)->parameters, ((Ad_AST_FunctionLiteral*)node)->body, &env);
            return obj;
        }
        break;
        case ST_CALL_EXPRESSION: {
            Ad_Object* func = Eval(((Ad_AST_CallExpression*)node)->function, env);
            if (IsError(func)) return func;
            std::vector<Ad_Object*> args_objs = EvalExpressions(((Ad_AST_CallExpression*)node)->arguments, env);
            if (args_objs.size() == 1 && IsError(args_objs[0])) {
                return args_objs[0];
            }
            Ad_Object* result = ApplyFunction(func, args_objs, env);
            //GarbageCollectEnvironments(); // this also doesn't work, i need to document why
            return result;
        }
        break;
        case ST_WHILE_EXPRESSION: {
            return EvalWhileExpression(node, env);
        }
        break;
        case ST_STRING_LITERAL: {
            return EvalString(node, env);
        }
        break;
        case ST_LIST_LITERAL: {
            std::vector<Ad_Object*> elements = EvalExpressions(((Ad_AST_ListLiteral*)node)->elements, env);
            Ad_List_Object* obj = new Ad_List_Object(elements);
            return obj;
        }
        break;
        case ST_INDEX_EXPRESSION: {
            Ad_Object* left = Eval(((Ad_AST_IndexExpression*)node)->left, env);
            if (IsError(left)) return left;
            Ad_Object* index = Eval(((Ad_AST_IndexExpression*)node)->index, env);
            if (IsError(index)) return index;
            return EvalIndexExpression(left, index);
        }
        break;
        case ST_HASH_LITERAL: {
            return EvalHashLiteral(node, env);
        }
        break;
        case ST_DEF_STATEMENT: {
            return EvalDefStatement(node, env);
        }
        break;
        case ST_COMMENT: {
            // this is just a comment, do nothing in the evaluator
            return NULL;
        }
        break;
        case ST_CLASS_STATEMENT: {
            return EvalClassStatement(node, env);
        }
        break;
        case ST_MEMBER_ACCESS: {
            return EvalMemberAccess(node, env);
        }
        break;
        case ST_PREFIX_INCREMENT:
            return EvalPrefixExpression(node, env);
        break;
        case ST_POSTFIX_INCREMENT:
            return EvalPostfixExpression(node, env);
        break;
        case ST_FOR_EXPRESSION:
            return EvalForExpression(node, env);
        break;
        case ST_BREAK_STATEMENT:
            return EvalBreakStatement(node, env);
        break;
        case ST_CONTINUE_STATEMENT:
            return EvalContinueStatement(node, env);
        break;
        case ST_NULL_EXPRESSION:
            return EvalNullExpression(node, env);
        break;
        default:
            std::cout << "unimplemented eval for token " << statement_type_map[node->type] << "\n";
        break;
    }
    return NULL;
}

Ad_Object* Evaluator::EvalProgram(Ad_AST_Node* node, Environment &env) {
    Init();
    Ad_Object* result;
    for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_AST_Program*)node)->statements.begin() ; it != ((Ad_AST_Program*)node)->statements.end(); ++it) {
        result = NULL;
        Ad_AST_Node *obj = *it;
        result = Eval(obj, env);
        //GarbageCollectEnvironments(); // commented this because garbage collecting after each statement might clear the environment before all the statements in the block got evaluated
        if (result != NULL) {
            //result->Print();
            if (result->Type() != OBJ_SIGNAL) {
                // no need to print signal objects
                std::cout << result->Inspect();
                std::cout << "\n";
                //std::cout << "object to be deleted: " << result->type << "\n";
            }
        }
        if (result != NULL && result->Type() == OBJ_SIGNAL) return result; // exit() builtin was used in order to trigger the stopping of the process
        if (result != NULL && result->Type() != OBJ_BUILTIN && result->ref_count <= 0) free_Ad_Object_memory(result); // TODO: remove OBJ_BUILTIN check and use ref_count
        // OBJ_BUILTINS get destroyed on termination by free_builtin_map
    }
    return NULL;
}

void Evaluator::Init() {
    NULLOBJECT.permanent = true;
    TRUE.permanent = true;
    FALSE.permanent = true;
}

void Evaluator::GarbageCollectEnvironments() {
    //std::cout << "cleaning up environments\n";
    garbageCollector.sweepEnvironments();
    for (Environment* env : environment_garbage_collection) {
        //delete env;
        //free_Ad_environment_memory(env);
    }
    //environment_garbage_collection.clear();
    garbageCollector.clearEnvironments();
}

Ad_Object* Evaluator::EvalInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    if (left == NULL || right == NULL) {
        return NULL;
    }
    if (left->Type() == OBJ_ERROR) {
        Ad_Error_Object* error_obj = (Ad_Error_Object*) left;
        return new Ad_Error_Object(error_obj->message);
    }
    if (right->Type() == OBJ_ERROR) {
        Ad_Error_Object* error_obj = (Ad_Error_Object*) right;
        return new Ad_Error_Object(error_obj->message);
    }
    if (left->Type() == OBJ_NULL || right->Type() == OBJ_NULL) {
        //return new Ad_Null_Object();
        return &NULLOBJECT;
    }
    if (left->Type() == OBJ_INT && right->Type() == OBJ_INT) {
        return EvalIntegerInfixExpression(_operator, left, right);
    }
    if (left->Type() == OBJ_BOOL && right->Type() == OBJ_BOOL) {
        return EvalBooleanInfixExpression(_operator, left, right);
    }
    if (left->Type() == OBJ_STRING && right->Type() == OBJ_STRING) {
        return EvalStringInfixExpression(_operator, left, right);
    }
    std::cout << "eval infix expression will return NULL\n";
    return NULL;
}

Ad_Object* Evaluator::EvalIntegerInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    if (left == NULL || right == NULL) {
        std::cout << "nu exista in env variabilele astea\n";
        //return &NULLOBJECT;
        return NULL;
    }
    int left_val = ((Ad_Integer_Object*)left)->value;
    int right_val = ((Ad_Integer_Object*)right)->value;
    if (_operator == "+") {
        Ad_Integer_Object* obj = new Ad_Integer_Object(left_val + right_val);
        return obj;
    }
    if (_operator == "-") {
        Ad_Integer_Object* obj = new Ad_Integer_Object(left_val - right_val);
        return obj;
    }
    if (_operator == "*") {
        Ad_Integer_Object* obj = new Ad_Integer_Object(left_val * right_val);
        return obj;
    }
    if (_operator == "/") {
        Ad_Integer_Object* obj = new Ad_Integer_Object(left_val / right_val);
        return obj;
    }
    if (_operator == "<") {
        return NativeBoolToBooleanObject(left_val < right_val);
    }
    if (_operator == ">") {
        return NativeBoolToBooleanObject(left_val > right_val);
    }
    if (_operator == "<=") {
        return NativeBoolToBooleanObject(left_val <= right_val);
    }
    if (_operator == ">=") {
        return NativeBoolToBooleanObject(left_val >= right_val);
    }
    if (_operator == "==") {
        return NativeBoolToBooleanObject(left_val == right_val);
    }
    if (_operator == "!=") {
        return NativeBoolToBooleanObject(left_val != right_val);
    }
    //return &NULLOBJECT;
    return NULL;
}

Ad_Object* Evaluator::EvalBooleanInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    bool left_val = ((Ad_Boolean_Object*)left)->value;
    bool right_val = ((Ad_Boolean_Object*)right)->value;
    if (_operator == "==") {
        return NativeBoolToBooleanObject(left_val == right_val);
    }
    if (_operator == "!=") {
        return NativeBoolToBooleanObject(left_val != right_val);
    }
    return NULL;
}

Ad_Object* Evaluator::EvalStringInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    std::string left_val = ((Ad_String_Object*)left)->value;
    std::string right_val = ((Ad_String_Object*)right)->value;
    if (_operator == "+") {
        Ad_String_Object* obj = new Ad_String_Object(left_val + right_val);
        return obj;
    }
    if (_operator == "==") {
        return NativeBoolToBooleanObject(left_val == right_val);
    }
    return NULL;
}

Ad_Object* Evaluator::EvalPrefixExpression(std::string _operator, Ad_Object* right) {
    if (_operator == "!") {
        return EvalBangOperatorExpression(right);
    }
    if (_operator == "-") {
        return EvalMinusPrefixOperatorExpression(right);
    }
    return NewError("unknown operator: " + _operator + object_type_map[right->type]);
}

Ad_Object* Evaluator::EvalBangOperatorExpression(Ad_Object* right) {
    if(!right) {
        return &TRUE;
    }
    if (((Ad_Boolean_Object*)right)->value == true) {
        return &FALSE;
    }
    if (((Ad_Boolean_Object*)right)->value == false) {
        return &TRUE;
    }
    return &FALSE;
}

Ad_Object* Evaluator::EvalMinusPrefixOperatorExpression(Ad_Object* right) {
    if (((Ad_Integer_Object*)right)->type != OBJ_INT) {
        return NULL;
    }
    int value = ((Ad_Integer_Object*)right)->value;
    Ad_Integer_Object* obj = new Ad_Integer_Object(-value);
    return obj;
}

Ad_Object* Evaluator::EvalIdentifier(Ad_AST_Node* node, Environment &env) {
    Ad_Object* obj;
    obj = NULL;
    if (env.Check(((Ad_AST_Identifier*)node)->token.literal)) {
        obj = env.Get(((Ad_AST_Identifier*)node)->token.literal);
        return obj;
    }
    if (builtins_map.find(((Ad_AST_Identifier*)node)->token.literal) != builtins_map.end()) {
        return builtins_map[((Ad_AST_Identifier*)node)->token.literal];
        //return NULL;
    }
    //obj = new Ad_Error_Object("ERROR: Identifier " + ((Ad_AST_Identifier*)node)->token.literal + " used before being declared.");
    obj = &NULLOBJECT;
    //obj = new Ad_Null_Object();
    return obj;
}

Ad_Object* Evaluator::EvalIfExpression(Ad_AST_Node* node, Environment &env) {
    Ad_Object* condition = Eval(((Ad_AST_IfExpression*)node)->condition, env);
    if (IsError(condition)) {
        return NULL;
    }
    if (IsTruthy(condition)) {
        return Eval(((Ad_AST_IfExpression*)node)->consequence, env);
    } else {
        if (((Ad_AST_IfExpression*)node)->alternative) {
            return Eval(((Ad_AST_IfExpression*)node)->alternative, env);
        }
    }
    return NULL;
}

Ad_Object* Evaluator::EvalBlockStatement(Ad_AST_Node* node, Environment &env) {
    Ad_Object* result;
    result = NULL;
    for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_AST_BlockStatement*)node)->statements.begin() ; it != ((Ad_AST_BlockStatement*)node)->statements.end(); ++it) {
        Ad_AST_Node *node = *it;
        result = NULL;
        result = Eval(node, env);
        if (result && result->type == OBJ_RETURN_VALUE) {
            return result;
        }
        if (result && result->type == OBJ_BREAK) {
            return result;
        }
        if (result && result->type == OBJ_CONTINUE) {
            return result;
        }
        if (FREE_BLOCK_STATEMENT_EVAL_STATEMENT_RESULTS) {
            if (result) {
                //result->Print();
                //std::cout << result->Inspect() << "\n"; // maybe have this on for command line flow and disabled for file execution
                free_Ad_Object_memory(result);
            }
        }
        // TODO: fix this, this is an important memory leak, test55.ad ran with valgrind has this issue, when doint i++ in a while block
        //free_Ad_Object_memory(result); // 11+12; as an expression in an if block or an while block needs to be freed
    }
    //return result;
    if (FREE_BLOCK_STATEMENT_EVAL_STATEMENT_RESULTS) {
        return NULL;
    } else {
        return result;
    }
}

std::vector<Ad_Object*> Evaluator::EvalExpressions(std::vector<Ad_AST_Node*> args, Environment &env) {
    std::vector<Ad_Object*> res;
    for (std::vector<Ad_AST_Node*>::iterator it = args.begin() ; it != args.end(); ++it) {
        Ad_AST_Node *node = *it;
        Ad_Object* evaluated = Eval(node, env);
        if (IsError(evaluated)) {
            std::vector<Ad_Object*> error_res;
            error_res.push_back(evaluated);
            return error_res;
        }
        res.push_back(evaluated);
    }
    return res;
}

Ad_Object* Evaluator::ApplyFunction(Ad_Object* func, std::vector<Ad_Object*> args, Environment &env) {
    if (func->type == OBJ_FUNCTION) {
        Ad_Function_Object* func_obj = (Ad_Function_Object*) func;
        if (func_obj->params.size() != args.size()) {
            for (int i = 0; i < args.size(); i++) free_Ad_Object_memory(args[i]);
            return new Ad_Error_Object("function signature unrecognized, different number of params");
        }
        Environment* extendedEnv = extendFunctionEnv(func, args);
        //Environment extendedEnv = ExtendFunctionEnv(func, args);
        Ad_Object* evaluated = Eval(func_obj->body, *extendedEnv);
        environment_garbage_collection.push_back(extendedEnv);
        garbageCollector.addEnvironment(extendedEnv);
        //std::cout << "se va returna un obiect de tipul: " << object_type_map[evaluated->type] << "\n";
        return UnwrapReturnValue(evaluated);
    }
    if (func->type == OBJ_BUILTIN) {
        Ad_Builtin_Object* builtinObject = (Ad_Builtin_Object*) func;
        /*if (builtinObject->acceptedNumbersOfArguments.size() != 0 &&
                !std::count(builtinObject->acceptedNumbersOfArguments.begin(),
                            builtinObject->acceptedNumbersOfArguments.end(),
                            args.size())) {
            return new Ad_Error_Object("builtin signature unrecognized, different number of params");
        }*/
        Ad_Object* result = builtinObject->builtin_function(args, &env);
        return result;
    }
    if (func->type == OBJ_CLASS) {
        Environment* instance_environment = new Environment();
        Ad_Class_Object* klass_object = (Ad_Class_Object*) func;
        Ad_AST_Identifier* klass_ident = (Ad_AST_Identifier*) klass_object->name;
        Ad_Class_Instance* klass_instance = new Ad_Class_Instance(klass_ident->value, klass_object, instance_environment);
        std::vector<Ad_AST_Node*> attributes = klass_object->attributes;
        for (std::vector<Ad_AST_Node*>::iterator it = attributes.begin(); it != attributes.end(); ++it) {
            Ad_AST_Node *node = *it;
            if (node->type == ST_ASSIGN_STATEMENT) {
                Ad_AST_AssignStatement* assign_statement = (Ad_AST_AssignStatement*) node;
                klass_instance->instance_environment->outer = &env;
                Ad_Object* evaluated = Eval(assign_statement->value, *klass_instance->instance_environment);
                Ad_AST_Identifier* assign_ident = (Ad_AST_Identifier*) assign_statement->name;
                std::string key = assign_ident->value;
                //klass_instance->instance_environment->Set(key, evaluated);
                klass_instance->instance_environment->SetCallArgument(key, evaluated);
            }
            if (node->type == ST_EXPRESSION_STATEMENT) {
                Ad_AST_ExpressionStatement * expression_statement = (Ad_AST_ExpressionStatement*) node;
                if (expression_statement->expression->type == ST_ASSIGN_STATEMENT) {
                    Ad_AST_AssignStatement* assign_statement = (Ad_AST_AssignStatement*) expression_statement->expression;
                    klass_instance->instance_environment->outer = &env;
                    Ad_Object* evaluated = Eval(assign_statement->value, *klass_instance->instance_environment);
                    Ad_AST_Identifier* assign_ident = (Ad_AST_Identifier*) assign_statement->name;
                    std::string key = assign_ident->value;
                    //std::cout << key << "\n";
                    //klass_instance->instance_environment->Set(key, evaluated);
                    klass_instance->instance_environment->SetCallArgument(key, evaluated);
                }
            }
        }
        std::vector<Ad_AST_Node*> methods = klass_object->methods;
        for (std::vector<Ad_AST_Node*>::iterator it = methods.begin(); it != methods.end(); ++it) {
            Ad_AST_Def_Statement* def_stmt = (Ad_AST_Def_Statement*) *it;
            Ad_Function_Object* method_obj = new Ad_Function_Object(def_stmt->parameters, def_stmt->body, klass_instance->instance_environment);
            Ad_AST_Identifier* def_ident = (Ad_AST_Identifier*) def_stmt->name;
            //std::cout << def_ident->value << "\n";
            klass_instance->instance_environment->Set(def_ident->value, method_obj);
        }
        CallInstanceConstructor(klass_instance, args, env);
        return klass_instance;
    }
    return NULL;
}

Ad_Object* Evaluator::CallInstanceConstructor(Ad_Object* klass_instance, std::vector<Ad_Object*> args, Environment &env) {
    Environment* instance_environment = ((Ad_Class_Instance*) klass_instance)->instance_environment;
    Ad_Object* klass_method = instance_environment->Get("constructor");
    if (klass_method != NULL) {
        // a method named constructor was found, calling it
        if (args.size() == 1 && IsError(args[0])) {
            return args[0];
        }
        instance_environment->outer = &env;
        return ApplyMethod(klass_method, args, *instance_environment);
    }
    return NULL;
}

Ad_Object* Evaluator::ApplyMethod(Ad_Object* func, std::vector<Ad_Object*> args, Environment &env) {
    if (func->type == OBJ_FUNCTION) {
        Environment* extendedEnv = ExtendMethodEnv(func, args, env);
        Ad_Object* evaluated = Eval(((Ad_Function_Object*)func)->body, *extendedEnv);
        environment_garbage_collection.push_back(extendedEnv);
        garbageCollector.addEnvironment(extendedEnv);
        return UnwrapReturnValue(evaluated);
    }
    return NULL;
}

Environment* Evaluator::ExtendMethodEnv(Ad_Object* func, std::vector<Ad_Object*> args_objs, Environment& env) {
    Ad_Function_Object* func_obj = (Ad_Function_Object*) func;
    //Environment extended = NewEnclosedEnvironment(&(*(func_obj)->env));
    Environment* extended = newEnclosedEnvironmentUnfreeable((func_obj)->env);
    int i = 0;
    for (std::vector<Ad_AST_Node*>::iterator it = func_obj->params.begin() ; it != func_obj->params.end(); ++it) {
        //env.Set((*it)->TokenLiteral(), args_objs[i]);
        extended->SetCallArgument((*it)->TokenLiteral(), args_objs[i]);
        ++i;
    }
    return extended;
}

Ad_Object* Evaluator::UnwrapReturnValue(Ad_Object* obj) {
    if (obj == NULL) return obj; // found when doing the Java implementation
    if (obj->Type() == OBJ_RETURN_VALUE) {
        Ad_Object* returned_obj = ((Ad_ReturnValue_Object*)obj)->value;
        free_Ad_Object_memory(obj);
        /*if (returned_obj->type == OBJ_FUNCTION) {
            std::cout << "pe aici!!!!\n";
            //Ad_INCREF(returned_obj);
        }*/
        return returned_obj;
    }
    return obj;
}

Environment Evaluator::ExtendFunctionEnv(Ad_Object* func, std::vector<Ad_Object*> args) {
    Ad_Function_Object* func_obj = (Ad_Function_Object*) func;
    Environment extended = NewEnclosedEnvironment(func_obj->env);
    int i = 0;
    for (std::vector<Ad_AST_Node*>::iterator it = func_obj->params.begin() ; it != func_obj->params.end(); ++it) {
        extended.SetCallArgument((*it)->TokenLiteral(), args[i++]);
    }
    return extended;
}

Environment* Evaluator::extendFunctionEnv(Ad_Object* func, std::vector<Ad_Object*> args) {
    Ad_Function_Object* func_obj = (Ad_Function_Object*) func;
    //Environment extended = NewEnclosedEnvironment(func_obj->env);
    Environment* extended = newEnclosedEnvironment(func_obj->env);
    int i = 0;
    for (std::vector<Ad_AST_Node*>::iterator it = func_obj->params.begin() ; it != func_obj->params.end(); ++it) {
        extended->SetCallArgument((*it)->TokenLiteral(), args[i++]);
    }
    return extended;
}

Ad_Object* Evaluator::EvalWhileExpression(Ad_AST_Node* node, Environment &env) {
    Ad_Object* condition = Eval(((Ad_AST_WhileExpression*)node)->condition, env);
    if (IsError(condition)) {
        return NULL;
    }
    while (IsTruthy(condition)) {
        // this needs to be handled just like EvalProgram or else i'll have a memory leak
        Ad_Object* result;
        result = Eval(((Ad_AST_WhileExpression*)node)->consequence, env);
        if (result != NULL && result->Type() == OBJ_SIGNAL) return result; // exit() builtin was used in order to trigger the stopping of the process
        if (result != NULL && result->Type() == OBJ_RETURN_VALUE) return result; // if a return is encountered in the block statement then that's the object the eval block must return
        if (result != NULL && result->Type() == OBJ_BREAK) {
            free_Ad_Object_memory(result);
            return NULL;
        }
        if (result != NULL && result->Type() == OBJ_CONTINUE) {
            free_Ad_Object_memory(result);
        }
        condition = Eval(((Ad_AST_WhileExpression*)node)->condition, env);
    }
    return NULL;
}

Ad_Object* Evaluator::EvalIndexExpression(Ad_Object* left, Ad_Object* index) {
    if (left->type == OBJ_LIST && index->type == OBJ_INT) {
        return EvalListIndexExpression(left, index);
    }
    if (left->type == OBJ_HASH) {
        return EvalHashIndexExpression(left, index);
    }
    // addig free calls here for freeing temp objects(like the index int) were allocated for evaluating an index expression
    free_Ad_Object_memory(left); // this should have ref_count > 0 if store in a context variable
    free_Ad_Object_memory(index);

    // this should return an Ad_Error_Object
    return NULL;
}

Ad_Object* Evaluator::EvalListIndexExpression(Ad_Object* left, Ad_Object* index) {
    int max = ((Ad_List_Object*)left)->elements.size();
    int idx = ((Ad_Integer_Object*)index)->value;
    if (idx < 0 || idx >= max) return NULL;
    free_Ad_Object_memory(left); // this should have ref_count > 0 if store in a context variable
    free_Ad_Object_memory(index);
    return ((Ad_List_Object*)left)->elements.at(idx);
}

Ad_Object* Evaluator::EvalHashLiteral(Ad_AST_Node* node, Environment &env) {
    std::map<std::string, HashPair> pairs;
    for(std::map<Ad_AST_Node*, Ad_AST_Node*>::iterator it = ((Ad_AST_HashLiteral*)node)->pairs.begin(); it != ((Ad_AST_HashLiteral*)node)->pairs.end(); ++it) {
        Ad_Object* key = Eval(it->first, env);
        if (IsError(key)) {
            return key;
        }
        Ad_Object* value = Eval(it->second, env);
        Ad_INCREF(key);
        Ad_INCREF(value);
        if (IsError(value)) {
            return value;
        }
        std::hash<std::string> hash_string;
        HashPair hash_pair(key, value);
        pairs.insert(std::make_pair(std::to_string(hash_string(key->Hash())), hash_pair)); // value needs to be a HashPair
    }
    Ad_Hash_Object* hash = new Ad_Hash_Object(pairs);
    return hash;
}

Ad_Object* Evaluator::EvalHashIndexExpression(Ad_Object* left, Ad_Object* index) {
    std::hash<std::string> hash_string;
    Ad_Object* result = ((Ad_Hash_Object*)left)->pairs[std::to_string(hash_string(index->Hash()))].value;

    free_Ad_Object_memory(left); // this should have ref_count > 0 if store in a context variable
    free_Ad_Object_memory(index);
    return result;
}

Ad_Object* Evaluator::EvalAssignStatement(Ad_AST_Node* node, Environment &env) {
    Ad_AST_AssignStatement* assign_statement = (Ad_AST_AssignStatement*)node;
    if (assign_statement->name->type == ST_INDEX_EXPRESSION) {
        return EvalIndexExpressionAssign(node, env);
    } else if (assign_statement->name->type == ST_MEMBER_ACCESS) {
        if (((Ad_AST_MemberAccess*)(assign_statement->name))->owner->type == ST_THIS_EXPRESSION) {
            Ad_AST_MemberAccess* member_access = (Ad_AST_MemberAccess*) assign_statement->name;
            Ad_AST_Node* klass_member = member_access->member;
            Ad_Object* obj = Eval(assign_statement->value, env);
            //env.Set(((Ad_AST_Identifier*)klass_member)->value, obj);
            env.outer->Set(((Ad_AST_Identifier*)klass_member)->value, obj);
        } else {
            Ad_AST_MemberAccess* member_access = (Ad_AST_MemberAccess*) assign_statement->name;
            Ad_AST_Identifier* owner = (Ad_AST_Identifier*) member_access->owner;
            Ad_Class_Instance* klass_instance = (Ad_Class_Instance*) env.Get(owner->value);
            Ad_AST_Node* klass_member = member_access->member;
            Environment* klass_environment = klass_instance->instance_environment;
            Ad_Object* obj = Eval(assign_statement->value, env);
            klass_environment->Set(((Ad_AST_Identifier*)klass_member)->value, obj);
        }
    } else {
        Ad_Object* obj = Eval(assign_statement->value, env);
        Ad_AST_Identifier* identifier = (Ad_AST_Identifier*)assign_statement->name;
        env.Set(identifier->value, obj);
    }
    return NULL;
}

Ad_Object* Evaluator::EvalIndexExpressionAssign(Ad_AST_Node* node, Environment &env) {
    Ad_AST_AssignStatement *assign_statement = (Ad_AST_AssignStatement*)node;
    Ad_AST_IndexExpression *index_expression = (Ad_AST_IndexExpression*)assign_statement->name;
    Ad_Object* obj = Eval(index_expression->left, env);
    if (IsError(obj)) return obj;
    Ad_Object* index = Eval(index_expression->index, env);
    if (IsError(index)) return index;
    if (obj->Type() == OBJ_LIST) {
        int idx = ((Ad_Integer_Object*)index)->value;
        Ad_Object* value = Eval(((Ad_AST_AssignStatement*)node)->value, env);
        Ad_List_Object* list_obj = (Ad_List_Object*)obj;
        Ad_Object* old_obj = list_obj->elements[idx];
        Ad_INCREF(value);
        list_obj->elements[idx] = value;
        free_Ad_Object_memory(index);
        Ad_DECREF(old_obj);
        free_Ad_Object_memory(old_obj);
    }
    if (obj->Type() == OBJ_HASH) {
        std::hash<std::string> hash_string;
        Ad_Object* value = Eval(((Ad_AST_AssignStatement*)node)->value, env);
        Ad_INCREF(index);
        Ad_INCREF(value);
        HashPair hash_pair(index, value);
        Ad_Hash_Object* hash_obj = (Ad_Hash_Object*)obj;
        std::string hash = std::to_string(hash_string(index->Hash()));

        std::map<std::string, HashPair>::iterator it = hash_obj->pairs.find(hash);

        if (it == hash_obj->pairs.end()) {
            hash_obj->pairs.insert(std::make_pair(hash, hash_pair));
        } else {
            HashPair old_hash_pair = it->second;
            Ad_DECREF(old_hash_pair.key);
            Ad_DECREF(old_hash_pair.value);
            free_Ad_Object_memory(old_hash_pair.key);
            free_Ad_Object_memory(old_hash_pair.value);
            it->second = hash_pair;
        }
    }
    return NULL;
}

Ad_Object* Evaluator::EvalDefStatement(Ad_AST_Node* node, Environment& env) {
    Ad_AST_Def_Statement* def_statement = (Ad_AST_Def_Statement*) node;

    std::vector<Ad_AST_Node*> parameters = def_statement->parameters;
    Ad_AST_Node* body = def_statement->body;

    Ad_AST_Identifier* ident = (Ad_AST_Identifier*) def_statement->name;
    Ad_Function_Object* func = new Ad_Function_Object(parameters, body, &env);
    env.Set(ident->value, func);
    return NULL; // this is correct, i don't want to print the function memory address on its definition statement
}

Ad_Object* Evaluator::EvalClassStatement(Ad_AST_Node* node, Environment& env) {
    Ad_AST_Class* class_node = (Ad_AST_Class*) node;
    Ad_Class_Object* klass_object = new Ad_Class_Object(class_node->name, class_node->methods, class_node->attributes, class_node);
    Ad_AST_Identifier* klass_ident = (Ad_AST_Identifier*)(class_node->name);
    if (IS_CONSOLE_RUN && env.isGlobalEnvironment) {
        klass_object->attemptASTNodesDeletion = true;
    }
    std::string klass_name = klass_ident->value;
    env.Set(klass_name, klass_object);
    return NULL;
}

Ad_Object* Evaluator::EvalMemberAccess(Ad_AST_Node* node, Environment& env) {
    Ad_AST_MemberAccess* member_access = (Ad_AST_MemberAccess*) node;

    Ad_Object* evaluated = NULL;
    evaluated = EvalFileObjectMethod(node, member_access->arguments, env);
    if (evaluated != NULL) return evaluated;

    evaluated = evalSocketObjectMethod(node, member_access->arguments, env);
    if (evaluated != NULL) return evaluated;

    if (member_access->owner->type == ST_THIS_EXPRESSION) {
        if (member_access->is_method) {
            Ad_Object* klass_method = env.Get(((Ad_AST_Identifier*)member_access->member)->value);
            std::vector<Ad_Object*> args_objs = EvalExpressions(member_access->arguments, env);
            if (args_objs.size() == 1 && IsError(args_objs[0])) {
                return args_objs[0];
            }
            return ApplyMethod(klass_method, args_objs, env);
        } else {
            evaluated = Eval(member_access->member, *env.outer);
        }
        return evaluated;
    } else {
        if (member_access->is_method) {
            Ad_AST_Identifier* owner = (Ad_AST_Identifier*) member_access->owner;
            Ad_AST_Identifier* member = (Ad_AST_Identifier*) member_access->member;
            Ad_Class_Instance* klass_instance = (Ad_Class_Instance*) env.Get(owner->value);

            Environment* klass_environment = klass_instance->instance_environment;
            //Environment* old = klass_environment->outer;
            klass_environment->outer = &env;
            //klass_environment->outer = NULL;

            Ad_Object* klass_method = klass_instance->instance_environment->Get(member->value);
            if (klass_method == NULL) {
                return &NULLOBJECT;
            }
            std::vector<Ad_Object*> args_objs = EvalExpressions(member_access->arguments, env);
            if (args_objs.size() == 1 && IsError(args_objs[0])) {
                return args_objs[0];
            }

            Ad_Object* result = ApplyMethod(klass_method, args_objs, *klass_environment);
            //klass_environment->outer = old;
            return result;
        } else {
            Ad_AST_Identifier* owner = (Ad_AST_Identifier*) member_access->owner;
            Ad_AST_Identifier* member = (Ad_AST_Identifier*) member_access->member;
            Ad_Class_Instance* klass_instance = (Ad_Class_Instance*) env.Get(owner->value);
            Environment* klass_environment = klass_instance->instance_environment;
            //klass_environment->outer = &env;
            Environment* old = klass_environment->outer;
            klass_environment->outer = NULL;
            Ad_Object* result = Eval(member, *klass_environment);
            klass_environment->outer = old;
            return result;
        }
    }
}

Ad_Object* Evaluator::EvalFileObjectMethod(Ad_AST_Node* node, std::vector<Ad_AST_Node*> args, Environment& env) {
    // TODO: implement this
    Ad_AST_MemberAccess* member_access = (Ad_AST_MemberAccess*) node;
    if (member_access->owner->type != ST_IDENTIFIER) {
        return NULL;
    }
    Ad_AST_Identifier* owner_ident = (Ad_AST_Identifier*) member_access->owner;
    Ad_Object* owner_obj_raw = env.Get(owner_ident->value);
    if (owner_obj_raw == NULL) return NULL;
    if (owner_obj_raw->type == OBJ_FILE) {
        Ad_File_Object* owner = (Ad_File_Object*) owner_obj_raw;
        std::string method_name = ((Ad_AST_Identifier*) member_access->member)->value;
        if (method_name == "read") {
            if (owner->_operator == "r") {
                std::string data = read_file_content(owner->filename);
                Ad_String_Object* result = new Ad_String_Object(data);
                return result;
            }
        }
        if (method_name == "write") {
            if (owner->_operator.find("w") != std::string::npos) {
                std::vector<Ad_Object*> args_objs = EvalExpressions(member_access->arguments, env); // this needs consolidated somehow, looks like crap
                // inspect on Ad_Object_String is bad, it needs to be printed out without "" in file writings
                write_file_content(owner->filename, args_objs[0]->Inspect());
                return &NULLOBJECT;
            } else if (owner->_operator.find("a") != std::string::npos) {
                std::vector<Ad_Object*> args_objs = EvalExpressions(member_access->arguments, env); // this needs consolidated somehow, looks like crap
                // inspect on Ad_Object_String is bad, it needs to be printed out without "" in file writings
                append_file_content(owner->filename, args_objs[0]->Inspect());
                return &NULLOBJECT;
            }
        }
    }
    return NULL;
}

Ad_Object* Evaluator::evalSocketObjectMethod(Ad_AST_Node* node, std::vector<Ad_AST_Node*> args, Environment& env) {
    //std::cout << "evaluating a socket object member access\n"; // commenting this out for tests to pass
    Ad_AST_MemberAccess* member_access = (Ad_AST_MemberAccess*) node;
    if (member_access->owner->type != ST_IDENTIFIER) {
        return NULL;
    }
    Ad_AST_Identifier* owner_ident = (Ad_AST_Identifier*) member_access->owner;
    Ad_Object* owner_obj_raw = env.Get(owner_ident->value);
    if (owner_obj_raw->type == OBJ_SOCKET) {
        return &NULLOBJECT;
    }
    return NULL;
}

Ad_Object* Evaluator::EvalPrefixExpression(Ad_AST_Node* node, Environment& env) {
    Ad_AST_PrefixIncrement* prefix_increment = (Ad_AST_PrefixIncrement*) node;
    Ad_AST_Identifier* ident = (Ad_AST_Identifier*) prefix_increment->name;
    Ad_Object* old_obj = env.Get(ident->value);
    if (old_obj->Type() == OBJ_INT) {
        int value = ((Ad_Integer_Object*) old_obj)->value;
        Ad_Integer_Object* new_obj = new Ad_Integer_Object(value + 1);
        env.Set(ident->value, new_obj);
        Ad_Integer_Object* result = new Ad_Integer_Object(value + 1);
        return result;
    }
    return &NULLOBJECT;
}

Ad_Object* Evaluator::EvalPostfixExpression(Ad_AST_Node* node, Environment& env) {
    Ad_AST_PostfixIncrement* postfix_increment = (Ad_AST_PostfixIncrement*) node;
    Ad_AST_Identifier* ident = (Ad_AST_Identifier*) postfix_increment->name;
    Ad_Object* old_obj = env.Get(ident->value);
    if (old_obj->Type() == OBJ_INT) {
        int value = ((Ad_Integer_Object*) old_obj)->value;
        Ad_Integer_Object* new_obj = new Ad_Integer_Object(value + 1);
        env.Set(ident->value, new_obj);
        Ad_Integer_Object* result = new Ad_Integer_Object(value);
        return result;
    }
    return &NULLOBJECT;
}

Ad_Object* Evaluator::EvalForExpression(Ad_AST_Node* node, Environment& env) {
    Ad_AST_ForExprssion* expr = (Ad_AST_ForExprssion*) node;
    Ad_Object* initialization = Eval(expr->initialization, env);
    Ad_Object* condition = Eval(expr->condition, env);
    while (IsTruthy(condition)) {
        Ad_Object* result = Eval(expr->body, env);
        if (result != NULL && result->Type() == OBJ_SIGNAL) return result;
        if (result != NULL && result->Type() == OBJ_RETURN_VALUE) return result;
        if (result != NULL && result->Type() == OBJ_BREAK) {
            free_Ad_Object_memory(result);
            return NULL;
        }
        if (result != NULL && result->Type() == OBJ_CONTINUE) {
            free_Ad_Object_memory(result);
        }
        Ad_Object* step = Eval(expr->step, env);
        condition = Eval(expr->condition, env);
    }
    return NULL;
}

Ad_Object* Evaluator::EvalBreakStatement(Ad_AST_Node* node, Environment& env) {
    return new Ad_Break_Object();
}

Ad_Object* Evaluator::EvalContinueStatement(Ad_AST_Node* node, Environment& env) {
    return new Ad_Continue_Object();
}

Ad_Object* Evaluator::EvalNullExpression(Ad_AST_Node* node, Environment& env) {
    return &NULLOBJECT;
}

Ad_Object* Evaluator::EvalFloatExpression(Ad_AST_Node* node, Environment& env) {
    Ad_Float_Object* obj = new Ad_Float_Object();
    obj->value = ((Ad_AST_Float*) node)->value;
    return obj;
}

bool Evaluator::IsTruthy(Ad_Object* obj) {
    if (obj == NULL) return false;
    if (obj == &NULLOBJECT) return false;
    if (obj == &TRUE) return true;
    if (obj == &FALSE) return false;
    return true;
}

bool Evaluator::IsError(Ad_Object* obj) {
    if (obj == NULL) return false;
    return obj->type == OBJ_ERROR;
}

Ad_Object* Evaluator::NativeBoolToBooleanObject(bool value) {
    if (value) return &TRUE;
    return &FALSE;
}

Ad_Object* Evaluator::EvalString(Ad_AST_Node* node, Environment &env) {
    Ad_String_Object* obj = new Ad_String_Object(((Ad_AST_String*)node)->value);
    return obj;
}

Ad_Object* Evaluator::NewError(std::string message) {
    Ad_Error_Object* obj = new Ad_Error_Object(message);
    return obj;
}
