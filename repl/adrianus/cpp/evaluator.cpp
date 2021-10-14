#include "evaluator.h"
#include "hashpair.cpp" // nu-mi place importul asta, as fi preferat sa import doar headerul
#include "builtins.cpp"

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
            return EvalBlockStatement(node, env);
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
            return ApplyFunction(func, args_objs);
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
        default:
            std::cout << "unimplemented eval for token " << statement_type_map[node->type] << "\n";
        break;
    }
    return NULL;
}

Ad_Object* Evaluator::EvalProgram(Ad_AST_Node* node, Environment &env) {
    Ad_Object* result;
    for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_AST_Program*)node)->statements.begin() ; it != ((Ad_AST_Program*)node)->statements.end(); ++it) {
        result = NULL;
        Ad_AST_Node *obj = *it;
        result = Eval(obj, env);
        if (result != NULL) {
            //result->Print();
            std::cout << result->Inspect();
            std::cout << "\n";
            //std::cout << "object to be deleted: " << result->type << "\n";
        }
        if (result != NULL && result->Type() == OBJ_SIGNAL) return result; // exit() builtin was used in order to trigger the stopping of the process
        if (result != NULL && result->ref_count <= 0) free_Ad_Object_memory(result);
    }
    return NULL;
}

Ad_Object* Evaluator::EvalInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    if (left->Type() == OBJ_INT && right->Type() == OBJ_INT) {
        return EvalIntegerInfixExpression(_operator, left, right);
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

Ad_Object* Evaluator::EvalStringInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    std::string left_val = ((Ad_String_Object*)left)->value;
    std::string right_val = ((Ad_String_Object*)right)->value;
    if (_operator == "+") {
        Ad_String_Object* obj = new Ad_String_Object(left_val + right_val);
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
    return obj;
}

Ad_Object* Evaluator::EvalIdentifier(Ad_AST_Node* node, Environment &env) {
    Ad_Object* obj;
    obj = NULL;
    if (env.Check(((Ad_AST_Identifier*)node)->token.literal)) {
        obj = env.Get(((Ad_AST_Identifier*)node)->token.literal);
    }
    if (builtins_map.find(((Ad_AST_Identifier*)node)->token.literal) != builtins_map.end()) {
        return builtins_map[((Ad_AST_Identifier*)node)->token.literal];
    }
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
        Ad_AST_Node *obj = *it;
        result = NULL;
        result = Eval(obj, env);
        if (result && result->type == OBJ_RETURN_VALUE) {
            return result;
        }
        free_Ad_Object_memory(result); // 11+12; as an expression in an if block or an while block needs to be freed
    }
    return result;
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

Ad_Object* Evaluator::ApplyFunction(Ad_Object* func, std::vector<Ad_Object*> args) {
    if (func->type == OBJ_FUNCTION) {
        Environment extendedEnv = ExtendFunctionEnv(func, args);
        Ad_Object* evaluated = Eval(((Ad_Function_Object*)func)->body, extendedEnv);
        return UnwrapReturnValue(evaluated);
    }
    if (func->type == OBJ_BUILTIN) {
        return ((Ad_Builtin_Object*)func)->builtin_function(args);
    }
    return NULL;
}

Ad_Object* Evaluator::UnwrapReturnValue(Ad_Object* obj) {
    if (obj->Type() == OBJ_RETURN_VALUE) {
        Ad_Object* returned_obj = ((Ad_ReturnValue_Object*)obj)->value;
        free_Ad_Object_memory(obj);
        return returned_obj;
    }
    return obj;
}

Environment Evaluator::ExtendFunctionEnv(Ad_Object* func, std::vector<Ad_Object*> args) {
    Environment extended = NewEnclosedEnvironment(&(*((Ad_Function_Object*)func)->env));
    int i = 0;
    for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_Function_Object*)func)->params.begin() ; it != ((Ad_Function_Object*)func)->params.end(); ++it) {
        //std::cout << (*it)->TokenLiteral() << "\n";
        extended.Set((*it)->TokenLiteral(), args[i]);
        //std::cout << statement_type_map[(*it)->type] << "\n";
        //extended.Set("x", args[i]); // aici e problema
        ++i;
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
    Ad_Function_Object* func = new Ad_Function_Object();
    Ad_AST_Def_Statement* def_statement = (Ad_AST_Def_Statement*) node;
    func->params = def_statement->parameters;
    func->body = def_statement->body;
    func->env = &env;
    Ad_AST_Identifier* ident = (Ad_AST_Identifier*) def_statement->name;
    env.Set(ident->value, func);
    return func;
}

bool Evaluator::IsTruthy(Ad_Object* obj) {
    if (obj == NULL) return false;
    if (obj == &NULLOBJECT) return false;
    if (obj == &TRUE) return true;
    if (obj == &FALSE) return false;
    return true;
}

bool Evaluator::IsError(Ad_Object* obj) {
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
