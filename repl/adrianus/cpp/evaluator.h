#ifndef __EVALUATOR_H
#define __EVALUATOR_H

#include "ast.h"
#include "objects.h"

class Evaluator {
public:
    Ad_Object* Eval(Ad_AST_Node* node);
    Ad_Object* EvalProgram(Ad_AST_Node* node);
    Ad_Object* EvalInfixExpression(std::string, Ad_Object*, Ad_Object*);
    Ad_Object* EvalIntegerInfixExpression(std::string, Ad_Object*, Ad_Object*);
};

#endif