#include "evaluator.h"
#include "utils.h"
#include "hashpair.cpp" // nu-mi place importul asta, as fi preferat sa import doar headerul
#include "builtins.cpp"
#include "gc.cpp"
#include "eval_utils.cpp"
#include "socket_utils.h"
#include "thread_utils.h"


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
            garbageCollector->addObject(obj);
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
            garbageCollector->addObject(obj);
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
            // TODO: mark and sweep cleanup
            //free_Ad_Object_memory(right);
            //free_Ad_Object_memory(left);
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
            garbageCollector->addObject(obj);
            // TODP: add obj to gc
            return obj;
        }
        break;
        case ST_CALL_EXPRESSION: {
            return evalCallExpression(node, &env);
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
            //std::cout << "adding a list to gc\n";
            garbageCollector->addObject(obj);
            return obj;
        }
        break;
        case ST_INDEX_EXPRESSION: {
            return evalIndexExpression(node, &env);
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
            return EvalPrefixIncrement(node, env);
        break;
        case ST_POSTFIX_INCREMENT:
            return EvalPostfixIncrement(node, env);
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
        case ST_THIS_EXPRESSION:
            return evalThisExpression(node, &env);
        break;
        default:
            std::cout << "unimplemented eval for token " << statement_type_map[node->type] << "\n";
        break;
    }
    return NULL;
}

Ad_Object* Evaluator::EvalProgram(Ad_AST_Node* node, Environment &env) {
    // for the next if to work include utils.cpp and comment out the test_parser.cpp import in main.cpp and the code references from main.cpp
    //if (node) {
    //    print_ast_nodes(node, 0);
    //}
    Init();
    Ad_Object* result;
    for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_AST_Program*)node)->statements.begin() ; it != ((Ad_AST_Program*)node)->statements.end(); ++it) {
        result = NULL;
        Ad_AST_Node *obj = *it;
        result = Eval(obj, env);
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
        if (result != NULL && result->Type() == OBJ_ERROR) {
            // TODO: mark and sweep cleanup
            //free_Ad_Object_memory(result);
            garbageCollector->markObjects();
            garbageCollector->sweepObjects();
            return NULL;
        }
        // TODO: mark and sweep cleanup
        //if (result != NULL && result->Type() != OBJ_BUILTIN && result->ref_count <= 0) free_Ad_Object_memory(result); // TODO: remove OBJ_BUILTIN check and use ref_count
        // OBJ_BUILTINS get destroyed on termination by free_builtin_map
        //std::cout << statement_type_map[obj->type] << "\n";
        GarbageCollectEnvironments(); // commented this because garbage collecting after each statement might clear the environment before all the statements in the block got evaluated
        garbageCollector->markObjects();
        garbageCollector->sweepObjects();
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
    garbageCollector->sweepEnvironments();
}

Ad_Object* Evaluator::EvalInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    if (left == NULL || right == NULL) {
        return NULL;
    }
    if (left->Type() == OBJ_ERROR) {
        Ad_Error_Object* error_obj = (Ad_Error_Object*) left;
        Ad_Error_Object *obj = new Ad_Error_Object(error_obj->message);
        garbageCollector->addObject(obj);
        return obj;
    }
    if (right->Type() == OBJ_ERROR) {
        Ad_Error_Object* error_obj = (Ad_Error_Object*) right;
        Ad_Error_Object *obj = new Ad_Error_Object(error_obj->message);
        garbageCollector->addObject(obj);
        return obj;
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
    if (left->Type() == OBJ_STRING && right->Type() == OBJ_INT) {
        return EvalStringAndIntInfixExpression(_operator, left, right);
    }
    if (left->Type() == OBJ_INT && right->Type() == OBJ_STRING) {
        return EvalIntAndStringInfixExpression(_operator, left, right);
    }
    if (left->Type() == OBJ_FLOAT && right->Type() == OBJ_FLOAT) {
        return EvalFloatInfixExpression(_operator, left, right);
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
        garbageCollector->addObject(obj);
        return obj;
    }
    if (_operator == "-") {
        Ad_Integer_Object* obj = new Ad_Integer_Object(left_val - right_val);
        garbageCollector->addObject(obj);
        return obj;
    }
    if (_operator == "*") {
        Ad_Integer_Object* obj = new Ad_Integer_Object(left_val * right_val);
        garbageCollector->addObject(obj);
        return obj;
    }
    if (_operator == "/") {
        Ad_Integer_Object* obj = new Ad_Integer_Object(left_val / right_val);
        garbageCollector->addObject(obj);
        return obj;
    }
    if (_operator == "%") {
        Ad_Integer_Object* obj = new Ad_Integer_Object(left_val % right_val);
        garbageCollector->addObject(obj);
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
        garbageCollector->addObject(obj);
        return obj;
    }
    if (_operator == "==") {
        return NativeBoolToBooleanObject(left_val == right_val);
    }
    if (_operator == "!=") {
        return NativeBoolToBooleanObject(left_val != right_val);
    }
    return NULL;
}

Ad_Object* Evaluator::EvalStringAndIntInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    std::string left_val = ((Ad_String_Object*)left)->value;
    int right_val = ((Ad_Integer_Object*)right)->value;
    if (_operator == "+") {
        Ad_String_Object* obj = new Ad_String_Object(left_val +  std::to_string(right_val));
        garbageCollector->addObject(obj);
        return obj;
    }
    return NULL;
}

Ad_Object* Evaluator::EvalIntAndStringInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    int left_val = ((Ad_Integer_Object*)left)->value;
    std::string right_val = ((Ad_String_Object*)right)->value;
    if (_operator == "+") {
        Ad_String_Object* obj = new Ad_String_Object(std::to_string(left_val) +  right_val);
        garbageCollector->addObject(obj);
        return obj;
    }
    return NULL;
}

Ad_Object* Evaluator::EvalFloatInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    float left_val = ((Ad_Float_Object*)left)->value;
    float right_val = ((Ad_Float_Object*)right)->value;
    if (_operator == "+") {
        Ad_Float_Object* obj = new Ad_Float_Object(left_val + right_val);
        garbageCollector->addObject(obj);
        return obj;
    }
    if (_operator == "-") {
        Ad_Float_Object* obj = new Ad_Float_Object(left_val - right_val);
        garbageCollector->addObject(obj);
        return obj;
    }
    if (_operator == "*") {
        Ad_Float_Object* obj = new Ad_Float_Object(left_val * right_val);
        garbageCollector->addObject(obj);
        return obj;
    }
    if (_operator == "/") {
        Ad_Float_Object* obj = new Ad_Float_Object(left_val / right_val);
        garbageCollector->addObject(obj);
        return obj;
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
    garbageCollector->addObject(obj);
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
    obj = new Ad_Error_Object("variable " + ((Ad_AST_Identifier*)node)->token.literal + " undefined.");
    garbageCollector->addObject(obj);
    //obj = &NULLOBJECT;
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
                // TODO: mark and sweep cleanup
                //free_Ad_Object_memory(result);
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
            // TODO: mark and sweep cleanup
            //for (int i = 0; i < args.size(); i++) free_Ad_Object_memory(args[i]);
            Ad_Error_Object *obj = new Ad_Error_Object("function signature unrecognized, different number of params");
            garbageCollector->addObject(obj);
            return obj;
        }
        Environment* extendedEnv = extendFunctionEnv(func, args);
        Ad_INCREF(extendedEnv);
        garbageCollector->addEnvironment(extendedEnv);
        garbageCollector->scheduleEnvironmentToDECREF(extendedEnv);
        Ad_Object* evaluated = Eval(func_obj->body, *extendedEnv);
        return UnwrapReturnValue(evaluated, extendedEnv);
    }
    if (func->type == OBJ_BUILTIN) {
        Ad_Builtin_Object* builtinObject = (Ad_Builtin_Object*) func;
        if (builtinObject->acceptedNumbersOfArguments.size() != 0 &&
                !validateNumberOfArguments(builtinObject->acceptedNumbersOfArguments, args.size())) {
            Ad_Error_Object *obj = new Ad_Error_Object("builtin signature unrecognized, different number of params");
            garbageCollector->addObject(obj);
            return obj;
        }
        Ad_Object* result = builtinObject->builtin_function(args, &env, garbageCollector);
        return result;
    }
    if (func->type == OBJ_CLASS) {
        Environment* instance_environment = new Environment();
        instance_environment->ref_count = 1;
        garbageCollector->addEnvironment(instance_environment);
        Ad_Class_Object* klass_object = (Ad_Class_Object*) func;
        Ad_AST_Identifier* klass_ident = (Ad_AST_Identifier*) klass_object->name;
        Ad_Class_Instance* klass_instance = new Ad_Class_Instance(klass_ident->value, klass_object, instance_environment);
        klass_instance->instance_environment->isInstanceEnvironment = true;
        klass_instance->instance_environment->owningInstanceEnvironment = klass_instance;
        garbageCollector->addObject(klass_instance);
        std::vector<Ad_AST_Node*> attributes = klass_object->attributes;
        updateInstanceWithInheritedClasses(klass_instance, env);
        for (std::vector<Ad_AST_Node*>::iterator it = attributes.begin(); it != attributes.end(); ++it) {
            Ad_AST_Node *node = *it;
            if (node->type == ST_ASSIGN_STATEMENT) {
                Ad_AST_AssignStatement* assign_statement = (Ad_AST_AssignStatement*) node;
                klass_instance->instance_environment->outer = &env;
                Ad_Object* evaluated = Eval(assign_statement->value, *klass_instance->instance_environment);
                Ad_AST_Identifier* assign_ident = (Ad_AST_Identifier*) assign_statement->name;
                std::string key = assign_ident->value;
                //klass_instance->instance_environment->Set(key, evaluated);
                klass_instance->instance_environment->setLocalParam(key, evaluated);
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
                    klass_instance->instance_environment->setLocalParam(key, evaluated);
                }
            }
        }
        std::vector<Ad_AST_Node*> methods = klass_object->methods;
        for (std::vector<Ad_AST_Node*>::iterator it = methods.begin(); it != methods.end(); ++it) {
            Ad_AST_Def_Statement* def_stmt = (Ad_AST_Def_Statement*) *it;
            Ad_Function_Object* method_obj = new Ad_Function_Object(def_stmt->parameters, def_stmt->body, klass_instance->instance_environment);
            garbageCollector->addObject(method_obj);
            Ad_AST_Identifier* def_ident = (Ad_AST_Identifier*) def_stmt->name;
            //std::cout << def_ident->value << "\n";
            klass_instance->instance_environment->setLocalParam(def_ident->value, method_obj);
        }
        Ad_Object* constructorReturn = CallInstanceConstructor(klass_instance, args, env);
        if (IsError(constructorReturn)) {
            // TODO: mark and sweep cleanup
            //free_Ad_Object_memory(klass_instance);
            return constructorReturn;
        }
        return klass_instance;
    }
    if (func->type == OBJ_NULL) {
        // TODO: mark and sweep cleanup
        //for (int i = 0; i < args.size(); i++) free_Ad_Object_memory(args[i]);
        Ad_Object *result = new Ad_Error_Object("function not found");
        garbageCollector->addObject(result);
        return result;
    }
    return NULL;
}

Ad_Object* Evaluator::CallInstanceConstructor(Ad_Object* klass_instance, std::vector<Ad_Object*> args, Environment &env) {
    Environment* instance_environment = ((Ad_Class_Instance*) klass_instance)->instance_environment;
    Ad_Object* klass_method = instance_environment->lookupConstructor();
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
        Ad_Function_Object* func_obj = (Ad_Function_Object*) func;
        if (func_obj->params.size() != args.size()) {
            Ad_Error_Object *obj = new Ad_Error_Object("some error message here");
            garbageCollector->addObject(obj);
            return obj;
        }
        Environment* extendedEnv = ExtendMethodEnv(func, args, env);
        Ad_Object* evaluated = Eval(((Ad_Function_Object*)func)->body, *extendedEnv);
        garbageCollector->addEnvironment(extendedEnv);
        return UnwrapReturnValue(evaluated, extendedEnv);
    }
    return NULL;
}

Environment* Evaluator::ExtendMethodEnv(Ad_Object* func, std::vector<Ad_Object*> args_objs, Environment& env) {
    Ad_Function_Object* func_obj = (Ad_Function_Object*) func;
    Environment* extended = newEnclosedEnvironmentUnfreeable((func_obj)->env);
    int i = 0;
    for (std::vector<Ad_AST_Node*>::iterator it = func_obj->params.begin() ; it != func_obj->params.end(); ++it) {
        extended->setLocalParam((*it)->TokenLiteral(), args_objs[i]);
        ++i;
    }
    return extended;
}

Ad_Object* Evaluator::UnwrapReturnValue(Ad_Object* obj, Environment *env) {
    if (obj == NULL) return obj; // found when doing the Java implementation
    if (obj->Type() == OBJ_RETURN_VALUE) {
        Ad_Object* returned_obj = ((Ad_ReturnValue_Object*)obj)->value;
        // TODO: mark and sweep cleanup
        //free_Ad_Object_memory(obj);
        if (returned_obj->type == OBJ_FUNCTION) {
            if (!env->isGlobalEnvironment) {
                /* function objects have a reference to their enclosing environment, so i need to make sure that environment is not garbage collected */
                Ad_INCREF(env);
            }
        }
        return returned_obj;
    }
    return obj;
}

/*Environment Evaluator::ExtendFunctionEnv(Ad_Object* func, std::vector<Ad_Object*> args) {
    Ad_Function_Object* func_obj = (Ad_Function_Object*) func;
    Environment extended = NewEnclosedEnvironment(func_obj->env);
    int i = 0;
    for (std::vector<Ad_AST_Node*>::iterator it = func_obj->params.begin() ; it != func_obj->params.end(); ++it) {
        extended.setLocalParam((*it)->TokenLiteral(), args[i++]);
    }
    return extended;
}*/

Environment* Evaluator::extendFunctionEnv(Ad_Object* func, std::vector<Ad_Object*> args) {
    Ad_Function_Object* func_obj = (Ad_Function_Object*) func;
    Environment* extended = newEnclosedEnvironment(func_obj->env);
    Ad_INCREF(func_obj->env);
    int i = 0;
    for (std::vector<Ad_AST_Node*>::iterator it = func_obj->params.begin() ; it != func_obj->params.end(); ++it) {
        extended->setLocalParam((*it)->TokenLiteral(), args[i++]);
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
            // TODO: mark and sweep cleanup
            //free_Ad_Object_memory(result);
            return NULL;
        }
        if (result != NULL && result->Type() == OBJ_CONTINUE) {
            // TODO: mark and sweep cleanup
            //free_Ad_Object_memory(result);
        }
        condition = Eval(((Ad_AST_WhileExpression*)node)->condition, env);
    }
    return NULL;
}

Ad_Object* Evaluator::evalIndexExpression(Ad_AST_Node* node, Environment* env) {

    Ad_Object* left = Eval(((Ad_AST_IndexExpression*)node)->left, *env);
    if (IsError(left)) return left;
    Ad_Object* index = Eval(((Ad_AST_IndexExpression*)node)->index, *env);
    if (IsError(index)) return index;

    if (left->type == OBJ_LIST && index->type == OBJ_INT) {
        return EvalListIndexExpression(left, index);
    }
    if (left->type == OBJ_HASH) {
        return EvalHashIndexExpression(left, index);
    }
    if (left->type == OBJ_STRING) {
        return evalStringIndexExpression(left, index);
    }
    // addig free calls here for freeing temp objects(like the index int) were allocated for evaluating an index expression
    // TODO: mark and sweep cleanup
    //free_Ad_Object_memory(left); // this should have ref_count > 0 if store in a context variable
    //free_Ad_Object_memory(index);

    // this should return an Ad_Error_Object
    return NULL;
}

Ad_Object* Evaluator::EvalListIndexExpression(Ad_Object* left, Ad_Object* index) {
    int max = ((Ad_List_Object*)left)->elements.size();
    int idx = ((Ad_Integer_Object*)index)->value;
    if (idx < 0 || idx >= max) return NULL;
    // TODO: mark and sweep cleanup
    //free_Ad_Object_memory(left); // this should have ref_count > 0 if store in a context variable
    //free_Ad_Object_memory(index);
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
    garbageCollector->addObject(hash);
    return hash;
}

Ad_Object* Evaluator::EvalHashIndexExpression(Ad_Object* left, Ad_Object* index) {
    std::hash<std::string> hash_string;
    Ad_Object* result = ((Ad_Hash_Object*)left)->pairs[std::to_string(hash_string(index->Hash()))].value;

    // TODO: mark and sweep cleanup
    //free_Ad_Object_memory(left); // this should have ref_count > 0 if store in a context variable
    //free_Ad_Object_memory(index);
    return result;
}

Ad_Object* Evaluator::evalStringIndexExpression(Ad_Object* left, Ad_Object* index) {
    Ad_String_Object* obj = (Ad_String_Object*) left;
    int idx = ((Ad_Integer_Object*)index)->value;
    if (idx < 0 || idx >= obj->value.size()) return &NULLOBJECT;
    Ad_Object* result = new Ad_String_Object(obj->value.substr(idx, 1));
    garbageCollector->addObject(result);
    // TODO: mark and sweep cleanup
    //free_Ad_Object_memory(left);
    //free_Ad_Object_memory(index);
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
            //env.outer->Set(((Ad_AST_Identifier*)klass_member)->value, obj);
            if (env.isInstanceEnvironment) {
                env.setLocalParam(((Ad_AST_Identifier*)klass_member)->value, obj);
            } else if (env.outer->isInstanceEnvironment) {
                env.outer->setLocalParam(((Ad_AST_Identifier*)klass_member)->value, obj);
            }
        } else if (((Ad_AST_MemberAccess*)(assign_statement->name))->owner->type == ST_SUPER_EXPRESSION) {
            //std::cout << "evaluating an assign statement on a super() expression\n";
            Ad_AST_MemberAccess *member_access = (Ad_AST_MemberAccess*) assign_statement->name;
            Ad_AST_Super_Expression *superExpression = (Ad_AST_Super_Expression*) member_access->owner;
            Environment *parentKlassEnv = env.outer->getSibling(superExpression->target->TokenLiteral());
            Ad_AST_Identifier *member = (Ad_AST_Identifier*) member_access->member;
            Ad_Object *obj = Eval(assign_statement->value, env);
            parentKlassEnv->Set(member->value, obj);
        } else if (((Ad_AST_MemberAccess*)(assign_statement->name))->owner->type == ST_MEMBER_ACCESS) {
            //std::cout << "do a recursive member access assignment\n";
            return recursiveMemberAccessAssign(node, &env);
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
        if (IsError(obj)) {
            return obj;
        }
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
        // TODO: mark and sweep cleanup
        //free_Ad_Object_memory(index);
        Ad_DECREF(old_obj);
        // TODO: mark and sweep cleanup
        //free_Ad_Object_memory(old_obj);
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
            // TODO: mark and sweep cleanup
            //free_Ad_Object_memory(old_hash_pair.key);
            //free_Ad_Object_memory(old_hash_pair.value);
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
    garbageCollector->addObject(func);
    env.Set(ident->value, func);
    return NULL; // this is correct, i don't want to print the function memory address on its definition statement
}

Ad_Object* Evaluator::EvalClassStatement(Ad_AST_Node* node, Environment& env) {
    Ad_AST_Class* class_node = (Ad_AST_Class*) node;
    Ad_Class_Object* klass_object = new Ad_Class_Object(class_node->name, class_node->methods, class_node->attributes, class_node);
    garbageCollector->addObject(klass_object);
    klass_object->inheritFrom = class_node->inheritFrom;
    Ad_AST_Identifier* klass_ident = (Ad_AST_Identifier*)(class_node->name);
    if ((IS_CONSOLE_RUN && env.isGlobalEnvironment) || env.isRunningImportCommand) {
        klass_object->attemptASTNodesDeletion = true;
    }
    if (env.isBootstrapEnvironment) {
        klass_object->attemptASTNodesDeletion = true;
    }
    std::string klass_name = klass_ident->value;
    env.Set(klass_name, klass_object);
    return NULL;
}

void Evaluator::updateInstanceWithInheritedClasses(Ad_Object* obj, Environment& env) {
    Ad_Class_Instance* adClassInstance = (Ad_Class_Instance*) obj;
    std::vector<Ad_AST_Node*> superKlasses = ((Ad_Class_Object*)adClassInstance->klass_object)->inheritFrom;
    for (int i = 0; i < superKlasses.size(); i++) {
        std::string identifier = superKlasses.at(i)->TokenLiteral();
        adClassInstance->inheritFrom.push_back(identifier);
        Environment *parentKlassEnv = newEnvironment();
        Ad_Class_Object* parentAdClassObject = (Ad_Class_Object*) env.Get(identifier);

        for (std::vector<Ad_AST_Node*>::iterator it = parentAdClassObject->attributes.begin(); it != parentAdClassObject->attributes.end(); ++it) {
            Ad_AST_Node *node = *it;
            if (node->type == ST_ASSIGN_STATEMENT) {
                // this adds everything to main class
                Ad_AST_AssignStatement* assign_statement = (Ad_AST_AssignStatement*) node;
                adClassInstance->instance_environment->outer = &env;
                Ad_Object* evaluated = Eval(assign_statement->value, *adClassInstance->instance_environment);
                Ad_AST_Identifier* assign_ident = (Ad_AST_Identifier*) assign_statement->name;
                std::string key = assign_ident->value;
                adClassInstance->instance_environment->setLocalParam(key, evaluated);

                // this adds everything to map of parent classes envs
                parentKlassEnv->setLocalParam(key, evaluated);
                adClassInstance->instance_environment->addSibling(identifier, parentKlassEnv);
            }
            if (node->type == ST_EXPRESSION_STATEMENT) {
                Ad_AST_ExpressionStatement * expression_statement = (Ad_AST_ExpressionStatement*) node;
                if (expression_statement->expression->type == ST_ASSIGN_STATEMENT) {
                    // this adds everything to main class
                    Ad_AST_AssignStatement* assign_statement = (Ad_AST_AssignStatement*) expression_statement->expression;
                    adClassInstance->instance_environment->outer = &env;
                    Ad_Object* evaluated = Eval(assign_statement->value, *adClassInstance->instance_environment);
                    Ad_AST_Identifier* assign_ident = (Ad_AST_Identifier*) assign_statement->name;
                    std::string key = assign_ident->value;
                    adClassInstance->instance_environment->setLocalParam(key, evaluated);

                    // this adds everything to map of parent classes envs
                    parentKlassEnv->setLocalParam(key, evaluated);
                    adClassInstance->instance_environment->addSibling(identifier, parentKlassEnv);
                }
            }
        }
        std::vector<Ad_AST_Node*> methods = parentAdClassObject->methods;
        for (std::vector<Ad_AST_Node*>::iterator it = parentAdClassObject->methods.begin(); it != parentAdClassObject->methods.end(); ++it) {
            // this adds everything to main class
            Ad_AST_Def_Statement* def_stmt = (Ad_AST_Def_Statement*) *it;
            Ad_Function_Object* method_obj = new Ad_Function_Object(def_stmt->parameters, def_stmt->body, adClassInstance->instance_environment);
            garbageCollector->addObject(method_obj);
            Ad_AST_Identifier* def_ident = (Ad_AST_Identifier*) def_stmt->name;
            adClassInstance->instance_environment->Set(def_ident->value, method_obj);

            // this adds everything to map of parent classes envs
            parentKlassEnv->Set(def_ident->value, method_obj);
            adClassInstance->instance_environment->addSibling(identifier, parentKlassEnv);
        }
    }
}

Ad_Object* Evaluator::EvalMemberAccess(Ad_AST_Node* node, Environment& env) { // TODO: cleanup aici si in acelsi loc la implementarea in java
    Ad_AST_MemberAccess* member_access = (Ad_AST_MemberAccess*) node;

    Ad_Object* evaluated = NULL;
    evaluated = EvalFileObjectMethod(node, member_access->arguments, env);
    //if (evaluated != NULL && evaluated == &NULLOBJECT) return NULL;
    if (evaluated != NULL) return evaluated;

    evaluated = evalSocketObjectMethod(node, member_access->arguments, env);
    if (evaluated != NULL && evaluated == &NULLOBJECT) return NULL;
    if (evaluated != NULL) return evaluated;

    evaluated = evalThreadObjectMethod(node, member_access->arguments, env);
    if (evaluated != NULL && evaluated == &NULLOBJECT) return NULL;
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
    } else if (member_access->owner->type == ST_SUPER_EXPRESSION) {
        if (member_access->is_method) {
            std::vector<Ad_Object*> args_objs = EvalExpressions(member_access->arguments, env);
            if (args_objs.size() == 1 && IsError(args_objs[0])) {
                return args_objs[0];
            }
            Ad_AST_Super_Expression *superExpression = (Ad_AST_Super_Expression*) member_access->owner;
            Environment *parentKlassEnv = env.outer->getSibling(superExpression->target->TokenLiteral());
            Ad_AST_Identifier *member = (Ad_AST_Identifier*) member_access->member;
            Ad_Function_Object *parentMethod = (Ad_Function_Object*) parentKlassEnv->Get(member->value);
            Ad_Object *result = ApplyMethod(parentMethod, args_objs, *env.outer);
            return result;
        } else {
            Ad_AST_Super_Expression *superExpression = (Ad_AST_Super_Expression*) member_access->owner;
            Environment *parentKlassEnv = env.outer->getSibling(superExpression->target->TokenLiteral());
            Ad_AST_Identifier *member = (Ad_AST_Identifier*) member_access->member;
            Ad_Object *result = Eval(member, *parentKlassEnv);
            return result;
        }
    } else {
        if (member_access->is_method) {
            if (member_access->owner->type == ST_MEMBER_ACCESS) {
                return evalRecursiveMemberAccessCall(member_access, env);
            }
            if (member_access->owner->type == ST_CALL_EXPRESSION) {
                return evalRecursiveMemberAccessCall(member_access, env);
            }
            Ad_AST_Identifier* owner = (Ad_AST_Identifier*) member_access->owner;
            Ad_AST_Identifier* member = (Ad_AST_Identifier*) member_access->member;
            Ad_Class_Instance* klass_instance = (Ad_Class_Instance*) env.Get(owner->value);

            Environment* klass_environment = klass_instance->instance_environment;
            //Environment* old = klass_environment->outer;
            klass_environment->outer = &env;
            //klass_environment->outer = NULL;

            Ad_Object* klass_method = klass_instance->instance_environment->Get(member->value);
            if (klass_method == NULL) {
                //return &NULLOBJECT;
                Ad_Error_Object *obj = new Ad_Error_Object("method " + member->value + " not found in class " + ((Ad_Class_Object*) klass_instance->klass_object)->name->TokenLiteral());
                garbageCollector->addObject(obj);
                return obj;
            }
            std::vector<Ad_Object*> args_objs = EvalExpressions(member_access->arguments, env);
            if (args_objs.size() == 1 && IsError(args_objs[0])) {
                return args_objs[0];
            }

            Ad_Object* result = ApplyMethod(klass_method, args_objs, *klass_environment);
            //klass_environment->outer = old;
            return result;
        } else {
            if (member_access->owner->type == ST_IDENTIFIER) {
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
            if (member_access->owner->type == ST_MEMBER_ACCESS) {
                return evalRecursiveMemberAccessCall(member_access, env);
            }
            if (member_access->owner->type == ST_CALL_EXPRESSION) {
                return evalRecursiveMemberAccessCall(member_access, env);
            }
        }
    }
    return NULL;
}

Ad_Object* Evaluator::evalRecursiveMemberAccessCall(Ad_AST_Node* node, Environment& env) {
    // TODO: recursive member access assign statement should work this way also, not only retrievals
    // TODO: add index expression as initial member access handler
    std::vector<Ad_AST_MemberAccess*> chainedMemberAccesses;
    while (node->type == ST_MEMBER_ACCESS) {
        chainedMemberAccesses.push_back((Ad_AST_MemberAccess*) node);
        node = ((Ad_AST_MemberAccess*) node)->owner;
    }

    Environment *currentEnvironment = &env; // super tare, daca fac currentEnv = env atunci cand metoda ajunge la final currentEnv(care e pe stack) face ::~Environment() de unde rezulta un seg fault cand &env face la randul lui ::~Environment()

    // initialize env
    Ad_AST_Node* initialMemberAccess = chainedMemberAccesses.at(0);
    while(initialMemberAccess->type == ST_MEMBER_ACCESS) {
        initialMemberAccess = ((Ad_AST_MemberAccess*) initialMemberAccess)->owner;
    }

    if (initialMemberAccess->type == ST_CALL_EXPRESSION) {
        Ad_Object* obj = Eval(initialMemberAccess, *currentEnvironment);
        if (obj->type == OBJ_INSTANCE) {
            Environment *old = currentEnvironment;
            currentEnvironment = ((Ad_Class_Instance*) obj)->instance_environment;
            currentEnvironment->SetOuterEnvironment(old);
        }
    }

    if (initialMemberAccess->type == ST_IDENTIFIER) {
        Ad_Object* obj = Eval(initialMemberAccess, *currentEnvironment);
        if (obj->type == OBJ_INSTANCE) {
            Environment *old = currentEnvironment;
            currentEnvironment = ((Ad_Class_Instance*) obj)->instance_environment;
            currentEnvironment->SetOuterEnvironment(old);
        }
    }

    if (initialMemberAccess->type == ST_INDEX_EXPRESSION) {
        Ad_Object* obj = Eval(initialMemberAccess, *currentEnvironment);
        if (obj->type == OBJ_INSTANCE) {
            Environment *old = currentEnvironment;
            currentEnvironment = ((Ad_Class_Instance*) obj)->instance_environment;
            currentEnvironment->SetOuterEnvironment(old);
        }
    }
    // end initialize env

    for (int i = chainedMemberAccesses.size() - 1; i >= 0; i--) {
        Ad_AST_MemberAccess* currentMemberAccess = (Ad_AST_MemberAccess*) chainedMemberAccesses.at(i);
        if (currentMemberAccess->is_method) {
            // am de a face cu un call
            Ad_Object* obj = Eval(currentMemberAccess->member, *currentEnvironment);
            if (obj->type == OBJ_FUNCTION) {
                std::vector<Ad_Object*> args_objs = EvalExpressions(currentMemberAccess->arguments, env);
                Ad_Object* obj2 = ApplyMethod(obj, args_objs, *(((Ad_Function_Object*) obj)->env));

                if (i == 0) {
                    return obj2;
                }
                if (obj2->type == OBJ_INSTANCE) {
                    Environment *old = currentEnvironment;
                    currentEnvironment = ((Ad_Class_Instance*) obj2)->instance_environment;
                    currentEnvironment->SetOuterEnvironment(old);
                }
            }
        } else {
            // am de a face cu un identificator
            Ad_Object* obj = Eval(currentMemberAccess->member, *currentEnvironment);

            if (i == 0) {
                return obj;
            }

            if (obj->type == OBJ_INSTANCE) {
                Environment *old = currentEnvironment;
                currentEnvironment = ((Ad_Class_Instance*) obj)->instance_environment;
                currentEnvironment->SetOuterEnvironment(old);
            }
        }
    }
    // daca am ajuns aici atunci nu cred ca e ok
    return NULL;
}

Ad_Object* Evaluator::recursiveMemberAccessAssign(Ad_AST_Node *node, Environment *env) {
    // TODO: clean this up, and the other recursive access method, this needs consolidated, and more elegant
    // TODO: add index expression as initial member access handler
    Ad_AST_AssignStatement *assignStatement = (Ad_AST_AssignStatement*) node;
    Ad_AST_MemberAccess *memberAccess = (Ad_AST_MemberAccess*) assignStatement->name;
    std::vector<Ad_AST_MemberAccess*> chainedMemberAccesses;
    // aici ceva trebuie facut diferit pentru ca *node pointeaza catre assign statement, nu catre un member access
    Ad_AST_Node *memberAccessNode = (Ad_AST_MemberAccess*) assignStatement->name;
    while (memberAccessNode->type == ST_MEMBER_ACCESS) {
        chainedMemberAccesses.push_back((Ad_AST_MemberAccess*) memberAccessNode);
        memberAccessNode = ((Ad_AST_MemberAccess*) memberAccessNode)->owner;
    }

    Environment *currentEnvironment = env; // super tare, daca fac currentEnv = env atunci cand metoda ajunge la final currentEnv(care e pe stack) face ::~Environment() de unde rezulta un seg fault cand &env face la randul lui ::~Environment()

    // initialize env
    Ad_AST_Node* initialMemberAccess = chainedMemberAccesses.at(0);
    while(initialMemberAccess->type == ST_MEMBER_ACCESS) {
        initialMemberAccess = ((Ad_AST_MemberAccess*) initialMemberAccess)->owner;
    }

    if (initialMemberAccess->type == ST_CALL_EXPRESSION) {
        Ad_Object* obj = Eval(initialMemberAccess, *currentEnvironment);
        if (obj->type == OBJ_INSTANCE) {
            Environment *old = currentEnvironment;
            currentEnvironment = ((Ad_Class_Instance*) obj)->instance_environment;
            currentEnvironment->SetOuterEnvironment(old);
        }
    }

    if (initialMemberAccess->type == ST_IDENTIFIER) {
        Ad_Object* obj = Eval(initialMemberAccess, *currentEnvironment);
        if (obj->type == OBJ_INSTANCE) {
            Environment *old = currentEnvironment;
            currentEnvironment = ((Ad_Class_Instance*) obj)->instance_environment;
            currentEnvironment->SetOuterEnvironment(old);
        }
    }

    if (initialMemberAccess->type == ST_IDENTIFIER) {
        Ad_Object* obj = Eval(initialMemberAccess, *currentEnvironment);
        if (obj->type == OBJ_INSTANCE) {
            Environment *old = currentEnvironment;
            currentEnvironment = ((Ad_Class_Instance*) obj)->instance_environment;
            currentEnvironment->SetOuterEnvironment(old);
        }
    }
    // end initialize env

    for (int i = chainedMemberAccesses.size() - 1; i >= 0; i--) {
        Ad_AST_MemberAccess* currentMemberAccess = (Ad_AST_MemberAccess*) chainedMemberAccesses.at(i);
        if (currentMemberAccess->is_method) {
            // am de a face cu un call
            Ad_Object* obj = Eval(currentMemberAccess->member, *currentEnvironment);
            if (obj->type == OBJ_FUNCTION) {
                std::vector<Ad_Object*> args_objs = EvalExpressions(currentMemberAccess->arguments, *env);
                Ad_Object* obj2 = ApplyMethod(obj, args_objs, *(((Ad_Function_Object*) obj)->env));

                if (i == 0) {
                    //return obj2;
                }
                if (obj2->type == OBJ_INSTANCE) {
                    Environment *old = currentEnvironment;
                    currentEnvironment = ((Ad_Class_Instance*) obj2)->instance_environment;
                    currentEnvironment->SetOuterEnvironment(old);
                }
            }
        } else {
            // am de a face cu un identificator
            Ad_Object* obj = Eval(currentMemberAccess->member, *currentEnvironment);

            if (i == 0) {
                //return obj;
            }

            if (obj->type == OBJ_INSTANCE) {
                Environment *old = currentEnvironment;
                currentEnvironment = ((Ad_Class_Instance*) obj)->instance_environment;
                currentEnvironment->SetOuterEnvironment(old);
            }
        }
    }

    Ad_Object *obj = Eval(assignStatement->value, *env);
    Ad_AST_Identifier *identifier = (Ad_AST_Identifier*) memberAccess->member;
    currentEnvironment->Set(identifier->value, obj);

    return NULL;
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
                garbageCollector->addObject(result);
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
    Ad_AST_MemberAccess* member_access = (Ad_AST_MemberAccess*) node;
    if (member_access->owner->type != ST_IDENTIFIER) {
        return NULL;
    }
    Ad_AST_Identifier* owner_ident = (Ad_AST_Identifier*) member_access->owner;
    Ad_AST_Identifier* member_ident = (Ad_AST_Identifier*) member_access->member;
    Ad_Object* owner_obj_raw = env.Get(owner_ident->value);
    if (owner_obj_raw->type == OBJ_SOCKET) {
        if (member_access->is_method) {
            if (member_ident->value == "create_server") {
                create_server(owner_obj_raw);
            }
            if (member_ident->value == "create_client") {
                create_client(owner_obj_raw);
            }
            if (member_ident->value == "accept") {
                Ad_Object* result = accept(owner_obj_raw);
                garbageCollector->addObject(result);
                return result;
            }
            if (member_ident->value == "send") {
                std::vector<Ad_Object*> args_obj = EvalExpressions(args, env);
                send(owner_obj_raw, args_obj.at(0));
            }
            if (member_ident->value == "read") {
                Ad_Object* result = read(owner_obj_raw);
                garbageCollector->addObject(result);
                return result;
            }
            if (member_ident->value == "close") {
                close(owner_obj_raw);
            }
        } else {
            //...
        }
        return &NULLOBJECT;
    }
    return NULL;
}

Ad_Object* Evaluator::evalThreadObjectMethod(Ad_AST_Node* node, std::vector<Ad_AST_Node*> args, Environment& env) {
    Ad_AST_MemberAccess* member_access = (Ad_AST_MemberAccess*) node;
    if (member_access->owner->type != ST_IDENTIFIER) {
        return NULL;
    }
    Ad_AST_Identifier* owner_ident = (Ad_AST_Identifier*) member_access->owner;
    Ad_AST_Identifier* member_ident = (Ad_AST_Identifier*) member_access->member;
    Ad_Object* owner_obj_raw = env.Get(owner_ident->value);
    if (owner_obj_raw->type == OBJ_THREAD) {
        if (member_access->is_method) {
            if (member_ident->value == "callback" || member_ident->value == "execute") {
                // add reference to function object
                std::vector<Ad_Object*> args_obj = EvalExpressions(args, env);
                thread_callback(owner_obj_raw, args_obj);
            }
            if (member_ident->value == "runAsync" || member_ident->value == "start") {
                // create socket and detach
                thread_async_run(owner_obj_raw, garbageCollector, env);
            }
            if (member_ident->value == "runBlocking" || member_ident->value == "join") {
                // create socket and join
                thread_blocking_run(owner_obj_raw, garbageCollector, env);
            }
            if (member_ident->value == "await") {
                thread_await(owner_obj_raw, garbageCollector, env);
                Ad_Thread_Object* thread_object = (Ad_Thread_Object*) owner_obj_raw;
                if (thread_object->result == NULL) {
                    return &NULLOBJECT;
                }
                return thread_object->result;
            }
        } else {
            //...
        }
        return &NULLOBJECT;
    }
    return NULL;
}

Ad_Object* Evaluator::EvalPrefixIncrement(Ad_AST_Node* node, Environment& env) {
    Ad_AST_PrefixIncrement* prefix_increment = (Ad_AST_PrefixIncrement*) node;
    Ad_AST_Identifier* ident = (Ad_AST_Identifier*) prefix_increment->name;
    Ad_Object* old_obj = env.Get(ident->value);
    if (old_obj->Type() == OBJ_INT) {
        int value = ((Ad_Integer_Object*) old_obj)->value;
        Ad_Integer_Object* new_obj = new Ad_Integer_Object(value + 1);
        garbageCollector->addObject(new_obj);
        env.Set(ident->value, new_obj);
        Ad_Integer_Object* result = new Ad_Integer_Object(value + 1);
        garbageCollector->addObject(result);
        return result;
    }
    return &NULLOBJECT;
}

Ad_Object* Evaluator::EvalPostfixIncrement(Ad_AST_Node* node, Environment& env) {
    Ad_AST_PostfixIncrement *expr = (Ad_AST_PostfixIncrement*) node;
    if (expr->name->type == ST_INDEX_EXPRESSION) {
        Ad_AST_IndexExpression *indexExpression = (Ad_AST_IndexExpression*) (expr->name);
        Ad_Object *old_obj = evalIndexExpression(indexExpression, &env);
        int value = ((Ad_Integer_Object*) old_obj)->value;
        if ("++" == expr->_operator) {
            Ad_Integer_Object *new_obj = new Ad_Integer_Object(value + 1);
            garbageCollector->addObject(new_obj);
            Ad_AST_Node *left = indexExpression->left;
            Ad_AST_Node *index = indexExpression->index;
            Ad_Object *left_obj = Eval(left, env);
            Ad_Object *index_obj = Eval(index, env);
            if (left_obj->type == OBJ_LIST) {
                int i = ((Ad_Integer_Object*) index_obj)->value;
                Ad_List_Object *target = (Ad_List_Object*) left_obj;
                target->elements[i] = new_obj;
                Ad_Integer_Object *returned_obj = new Ad_Integer_Object(value);
                garbageCollector->addObject(returned_obj);
                return returned_obj;
            }
            if (left_obj->type == OBJ_HASH) {
                std::hash<std::string> hash_string;
                Ad_Hash_Object *target = (Ad_Hash_Object*) left_obj;

                HashPair hash_pair(index_obj, new_obj);
                std::string hash = std::to_string(hash_string(index_obj->Hash()));

                std::map<std::string, HashPair>::iterator it = target->pairs.find(hash);

                if (it == target->pairs.end()) {
                    target->pairs.insert(std::make_pair(hash, hash_pair));
                } else {
                    HashPair old_hash_pair = it->second;
                    it->second = hash_pair;
                }

                Ad_Integer_Object *returned_obj = new Ad_Integer_Object(value);
                garbageCollector->addObject(returned_obj);
                return returned_obj;
            }
        }
    }
    Ad_AST_Identifier* ident = (Ad_AST_Identifier*) expr->name;
    Ad_Object* old_obj = env.Get(ident->value);
    if (old_obj->Type() == OBJ_INT) {
        int value = ((Ad_Integer_Object*) old_obj)->value;
        Ad_Integer_Object* new_obj = new Ad_Integer_Object(value + 1);
        garbageCollector->addObject(new_obj);
        env.Set(ident->value, new_obj);
        Ad_Integer_Object* result = new Ad_Integer_Object(value);
        garbageCollector->addObject(result);
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
            // TODO: mark and sweep cleanup
            //free_Ad_Object_memory(result);
            return NULL;
        }
        if (result != NULL && result->Type() == OBJ_CONTINUE) {
            // TODO: mark and sweep cleanup
            //free_Ad_Object_memory(result);
        }
        Ad_Object* step = Eval(expr->step, env);
        // TODO: mark and sweep cleanup
        //free_Ad_Object_memory(step);
        condition = Eval(expr->condition, env);
    }
    return NULL;
}

Ad_Object* Evaluator::EvalBreakStatement(Ad_AST_Node* node, Environment& env) {
    Ad_Break_Object *result = new Ad_Break_Object();
    garbageCollector->addObject(result);
    return result;
}

Ad_Object* Evaluator::EvalContinueStatement(Ad_AST_Node* node, Environment& env) {
    Ad_Continue_Object *result = new Ad_Continue_Object();
    garbageCollector->addObject(result);
    return result;
}

Ad_Object* Evaluator::evalCallExpression(Ad_AST_Node* node, Environment *env) {
    Ad_AST_CallExpression *callExpression = (Ad_AST_CallExpression*) node;
    Ad_Object* func = Eval(callExpression->function, *env);
    if (IsError(func)) return func;
    if (func->type == OBJ_NULL) {
        // function was not found
        Ad_Error_Object *result = new Ad_Error_Object("function " + ((Ad_AST_Identifier*)callExpression->function)->value + " not found.");
        garbageCollector->addObject(result);
        return result;
    }
    std::vector<Ad_Object*> args_objs = EvalExpressions(callExpression->arguments, *env);
    if (args_objs.size() == 1 && IsError(args_objs[0])) {
        return args_objs[0];
    }
    Ad_Object* result = ApplyFunction(func, args_objs, *env);
    //GarbageCollectEnvironments(); // this also doesn't work, i need to document why, why is this not working?!?
    //Ad_DECREF(((Ad_Function_Object*)func)->env);
    garbageCollector->consumeScheduledDECREFEnvironments();
    return result;
}

Ad_Object* Evaluator::EvalNullExpression(Ad_AST_Node* node, Environment& env) {
    return &NULLOBJECT;
}

Ad_Object* Evaluator::EvalFloatExpression(Ad_AST_Node* node, Environment& env) {
    Ad_Float_Object* obj = new Ad_Float_Object();
    garbageCollector->addObject(obj);
    obj->value = ((Ad_AST_Float*) node)->value;
    return obj;
}

Ad_Object* Evaluator::evalThisExpression(Ad_AST_Node* node, Environment *env) {
    if (env->isInstanceEnvironment) {
        return env->owningInstanceEnvironment;
    }
    if (env->outer->isInstanceEnvironment) {
        return env->outer->owningInstanceEnvironment;
    }
    return NULL;
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
    garbageCollector->addObject(obj);
    return obj;
}

Ad_Object* Evaluator::NewError(std::string message) {
    Ad_Error_Object* obj = new Ad_Error_Object(message);
    garbageCollector->addObject(obj);
    return obj;
}

bool Evaluator::validateNumberOfArguments(std::vector<int> accepterNumberArguments, int argumentSize) {
    for (std::vector<int>::iterator it = accepterNumberArguments.begin() ; it != accepterNumberArguments.end(); ++it) {
        if (*it == argumentSize) return true;
    }
    return false;
}


void Evaluator::setGarbageCollector(GarbageCollector *gc) {
    garbageCollector = gc;
}