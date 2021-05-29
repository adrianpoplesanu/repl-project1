#include "evaluator.h"

Ad_Object* Evaluator::Eval(Ad_AST_Node* node) {
    switch(node->type) {
        case ST_PROGRAM:
            //std::cout << "evaluating ST_PROGRAM\n";
            return EvalProgram(node);
        break;
        case ST_EXPRESSION_STATEMENT:
            //std::cout << "evaluating ST_EXPRESSION_STATEMENT\n";
            return Eval(((Ad_AST_ExpressionStatement*)node)->expression);
        break;
        case ST_INFIX_EXPRESSION: {
            /*left = Eval(node.left, env)
            if isError(left):
                return left
            right = Eval(node.right, env)
            if isError(right):
                return right
            return evalInfixExpression(node.operator, left, right)*/
            //std::cout << "evaluating ST_INFIX_EXPRESSION\n";
            Ad_Object* left = Eval(((Ad_AST_InfixExpression*)node)->left);
            Ad_Object* right = Eval(((Ad_AST_InfixExpression*)node)->right);
            return EvalInfixExpression(((Ad_AST_InfixExpression*)node)->_operator, left, right);
            //break;
        }
        case ST_INTEGER: {
            //std::cout << "evaluating ST_INTEGER\n";
            Ad_Integer_Object* obj = new Ad_Integer_Object();
            obj->value = ((Ad_AST_Integer*)node)->value;
            return obj;
        }
        default:
            std::cout << node->type << "\n";
        break;
    }
    return NULL;
}

Ad_Object* Evaluator::EvalProgram(Ad_AST_Node* node) {
    //...
    Ad_Object* result;
    for (std::vector<Ad_AST_Node*>::iterator it = ((Ad_AST_Program*)node)->statements.begin() ; it != ((Ad_AST_Program*)node)->statements.end(); ++it) {
        Ad_AST_Node *obj = *it;
        result = Eval(obj);
        if (result) {
            //std::cout << result->type << "\n";
            //std::cout << result->value << "\n";
            result->Print();
            std::cout << "\n";
        }
    }

    return NULL;
}

Ad_Object* Evaluator::EvalInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    /*
    if left.Type() == ObjectType.INTEGER_OBJ and right.Type() == ObjectType.INTEGER_OBJ:
        return evalIntegerInfixExpression(operator, left, right)
    */
    if (left->Type() == OBJ_INT && right->Type() == OBJ_INT) {
        return EvalIntegerInfixExpression(_operator, left, right);
    }
    return NULL;
}

Ad_Object* Evaluator::EvalIntegerInfixExpression(std::string _operator, Ad_Object* left, Ad_Object* right) {
    /*
    leftVal = left.Value
    rightVal = right.Value
    if operator == '+':
        return IntegerObject(Value=leftVal+rightVal)
    elif operator == '-':
        return IntegerObject(Value=leftVal-rightVal)
    elif operator == '/':
        return IntegerObject(Value=leftVal/rightVal)
    elif operator == '*':
        return IntegerObject(Value=leftVal*rightVal)
    */
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