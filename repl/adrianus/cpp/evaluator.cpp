#include "evaluator.h"

Ad_Null_Object NULLOBJECT;
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
        case ST_BOOLEAN: {
            //Ad_Boolean_Object* obj = new Ad_Boolean_Object();
            //obj->value = ((Ad_AST_Boolean*)node)->value;
            //return obj;
            return NativeBoolToBooleanObject(((Ad_AST_Boolean*)node)->value);
        }
        break;
        case ST_INFIX_EXPRESSION: {
            Ad_Object* left = Eval(((Ad_AST_InfixExpression*)node)->left, env);
            Ad_Object* right = Eval(((Ad_AST_InfixExpression*)node)->right, env);
            Ad_Object* result = EvalInfixExpression(((Ad_AST_InfixExpression*)node)->_operator, left, right);
            // this object deletion process needs to be linked with env clear up and removal of references
            // probably Ad_INCREF and Ad_DECREF will need to be used here
            //delete left; // this is smart but here it just deletes pointers that are still referenced in the env
            //delete right; // this is smart but here it just deletes pointers that are still referenced in the env
            // PROBLEMA e cand left sau right sunt luate din env, crapa pe macos
            //free_Ad_Object_memory(left); // this should be fine?, it's an object created based on an AST node, and i guess it's not still refenreced
            //free_Ad_Object_memory(right); // this should be fine?, it's an object created based on an AST node, and i guess it's not still refenreced
            //std::cout << ((Ad_AST_InfixExpression*)node)->left->type << "\n";
            //std::cout << ((Ad_AST_InfixExpression*)node)->right->type << "\n";
            //if (((Ad_AST_InfixExpression*)node)->left->type == ST_INTEGER) free_Ad_Object_memory(left); // macos vrea if-ul asta, in linux nu e nicio problema
            //if (((Ad_AST_InfixExpression*)node)->right->type == ST_INTEGER) free_Ad_Object_memory(right); // macos vrea if-ul asta, in linux nu e nicio problema
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
            return EvalBlockStatement(node, env);
        }
        break;
        case ST_FUNCTION_LITERAL: {
            std::cout << "function literal eval\n";
            Ad_Function_Object* obj = new Ad_Function_Object(((Ad_AST_FunctionLiteral*)node)->parameters, ((Ad_AST_FunctionLiteral*)node)->body, &env);
            return obj;
        }
        break;
        case ST_CALL_EXPRESSION: {
            std::cout << "call expression eval\n";
            /*
            function = Eval(node.function, env)
            if isError(function):
                return function
            args = evalExpressions(node.arguments, env)
            if len(args) == 1 and isError(args[0]):
                return args[0]
            return applyFunction(function, args)
            */
            Ad_Object* func = Eval(((Ad_AST_CallExpression*)node)->function, env);
            if (IsError(func)) return func;
            std::vector<Ad_Object*> args_objs = EvalExpressions(((Ad_AST_CallExpression*)node)->arguments, env);
            if (args_objs.size() == 1 && IsError(args_objs[0])) {
                return args_objs[0];
            }
            return ApplyFunction(func, args_objs);
        }
        break;
        case ST_WHILE_EXPRESSION: {

        }
        break;
        default:
            std::cout << "unimplemented eval for token " << statement_type_map[node->type] << "\n";
        break;
    }
    return NULL;
}

Ad_Object* Evaluator::EvalProgram(Ad_AST_Node* node, Environment &env) {
    Ad_Object* result;
    for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_AST_Program*)node)->statements.begin() ; it != ((Ad_AST_Program*)node)->statements.end(); ++it) {
        Ad_AST_Node *obj = *it;
        result = Eval(obj, env);
        if (result) {
            //result->Print();
            std::cout << result->Inspect();
            std::cout << "\n";
            //std::cout << "object to be deleted: " << result->type << "\n";
        }
        //delete result; // i don't think this needs to be here, it needs to be binded with the env if it's an assignment
        //if (result) delete(result); // i will need to bind this to the env, but how? also, maybe this needs to be free_Ad_Object_memory()
    }

    return NULL;
}

Ad_Object* Evaluator::EvalInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    if (left->Type() == OBJ_INT && right->Type() == OBJ_INT) {
        return EvalIntegerInfixExpression(_operator, left, right);
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
    return env.Get(((Ad_AST_Identifier*)node)->token.literal);
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
    for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_AST_BlockStatement*)node)->statements.begin() ; it != ((Ad_AST_BlockStatement*)node)->statements.end(); ++it) {
        Ad_AST_Node *obj = *it;
        result = Eval(obj, env);
        if (result->type == OBJ_RETURN_VALUE) {
            std::cout << "encountered a return statements, need to stop and return this";
        }
    }
    return result;
}

std::vector<Ad_Object*> Evaluator::EvalExpressions(std::vector<Ad_AST_Node*> args, Environment &env) {
    std::vector<Ad_Object*> res;
    for (std::vector<Ad_AST_Node*>::iterator it = args.begin() ; it != args.end(); ++it) {
        Ad_AST_Node *obj = *it;
        Ad_Object* evaluated = Eval(obj, env);
        if (IsError(evaluated)) {
            std::vector<Ad_Object*> error_res;
            error_res.push_back(evaluated);
            return error_res;
        }
        res.push_back(evaluated);
    }
    return res;
}

/*
def applyFunction(fn, args):
    if type(fn) == FunctionObject:
        extendedEnv = extendFunctionEnv(fn, args)
        evaluated = Eval(fn.body, extendedEnv)
        return unwrapReturnValue(evaluated)
    elif type(fn) == BuiltinObject:
        return fn.builtin_function(*args)
    return newError("not a function {0}".format(fn.Type()))

def extendFunctionEnv(fn, args):
    env = NewEnclosedEnvironment(fn.env)

    for i, param in enumerate(fn.parameters):
        env.Set(param.value, args[i])
    return env

def unwrapReturnValue(obj):
    if type(obj) == ReturnValue:
        return obj.Value
    return obj
*/

Ad_Object* Evaluator::ApplyFunction(Ad_Object* func, std::vector<Ad_Object*> args) {
    if (func->type == OBJ_FUNCTION) {
        Environment extendedEnv = ExtendFunctionEnv(func, args);
        Ad_Object* evaluated = Eval(((Ad_Function_Object*)func)->body, *((Ad_Function_Object*)func)->env);
        return evaluated;
    }
    return NULL;
}

Ad_Object* Evaluator::UnwrapReturnValue(Ad_Object* obj) {
    if (obj->Type() == OBJ_RETURN_VALUE) {
        return ((Ad_ReturnValue_Object*)obj)->value;
    }
    return obj;
}

Environment Evaluator::ExtendFunctionEnv(Ad_Object* func, std::vector<Ad_Object*> args) {
    Environment extended = NewEnclosedEnvironment(*((Ad_Function_Object*)func)->env);
    int i = 0;
    for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_Function_Object*)func)->params.begin() ; it != ((Ad_Function_Object*)func)->params.end(); ++it) {
        extended.Set((*it)->TokenLiteral(), args[i]);
        ++i;
    }
    return extended;
}

bool Evaluator::IsTruthy(Ad_Object* obj) {
    if (obj == NULL) return false;
    if (obj == &NULLOBJECT) return false;
    if (obj == &TRUE) return true;
    if (obj == &FALSE) return false;
    return true;
}

bool Evaluator::IsError(Ad_Object* obj) {
    std::cout << "error check\n";
    return obj->type == OBJ_ERROR;
}

Ad_Object* Evaluator::NativeBoolToBooleanObject(bool value) {
    if (value) return &TRUE;
    return &FALSE;
}

Ad_Object* Evaluator::NewError(std::string message) {
    Ad_Error_Object* obj = new Ad_Error_Object(message);
    return obj;
}