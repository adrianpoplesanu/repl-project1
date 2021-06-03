#include "evaluator.h"

Ad_Boolean_Object TRUE(true);
Ad_Boolean_Object FALSE(false);

Ad_Object* Evaluator::Eval(Ad_AST_Node* node) {
    switch(node->type) {
        case ST_PROGRAM:
            return EvalProgram(node);
        break;
        case ST_EXPRESSION_STATEMENT:
            if (((Ad_AST_ExpressionStatement*)node)->expression) {
                return Eval(((Ad_AST_ExpressionStatement*)node)->expression);
            }
        break;
        case ST_INFIX_EXPRESSION: {
            Ad_Object* left = Eval(((Ad_AST_InfixExpression*)node)->left);
            Ad_Object* right = Eval(((Ad_AST_InfixExpression*)node)->right);
            Ad_Object* result = EvalInfixExpression(((Ad_AST_InfixExpression*)node)->_operator, left, right);
            delete left; // this is smart
            delete right; // this is smart
            return result;
        }
        case ST_INTEGER: {
            Ad_Integer_Object* obj = new Ad_Integer_Object();
            obj->value = ((Ad_AST_Integer*)node)->value;
            return obj;
        }
        case ST_BOOLEAN: {
            Ad_Boolean_Object* obj = new Ad_Boolean_Object();
            obj->value = ((Ad_AST_Boolean*)node)->value;
            return obj;
        }
        case ST_PREFIX_EXPRESSION: {
            Ad_Object* right = Eval(((Ad_AST_PefixExpression*)node)->right);
            return EvalPrefixExpression(((Ad_AST_PefixExpression*)node)->_operator, right);
        }
        default:
            std::cout << "unimplemented eval for token " << statement_type_map[node->type] << "\n";
        break;
    }
    return NULL;
}

Ad_Object* Evaluator::EvalProgram(Ad_AST_Node* node) {
    Ad_Object* result;
    for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_AST_Program*)node)->statements.begin() ; it != ((Ad_AST_Program*)node)->statements.end(); ++it) {
        Ad_AST_Node *obj = *it;
        result = Eval(obj);
        if (result) {
            //result->Print();
            std::cout << result->Inspect();
            std::cout << "\n";
            //std::cout << "object to be deleted: " << result->type << "\n";
        }
        //delete result; // i don't think this needs to be here, it needs to be binded with the env if it's an assignment
    }

    return NULL;
}

Ad_Object* Evaluator::EvalInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    if (left->Type() == OBJ_INT && right->Type() == OBJ_INT) {
        return EvalIntegerInfixExpression(_operator, left, right);
    }
    return NULL;
}

Ad_Object* Evaluator::EvalIntegerInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
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
    return NULL;
}

Ad_Object* Evaluator::EvalPrefixExpression(std::string _operator, Ad_Object* right) {
    if (_operator == "!") {
        return EvalBangOperatorExpression(right);
    }
    if (_operator == "-") {
        return EvalMinusPrefixOperatorExpression(right);
    }
    //return NewError("unknown operator: " + _operator + object_type_map[right->type]);
    return NewError("prefix expression error");
}

Ad_Object* Evaluator::EvalBangOperatorExpression(Ad_Object* right) {
    return NULL;
}

Ad_Object* Evaluator::EvalMinusPrefixOperatorExpression(Ad_Object* right) {
    return NULL;
}

Ad_Object* Evaluator::NewError(std::string message) {
    Ad_Error_Object* obj = new Ad_Error_Object(message);
    return obj;
}