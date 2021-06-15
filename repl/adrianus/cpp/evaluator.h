#ifndef __EVALUATOR_H
#define __EVALUATOR_H

#include "ast.h"
#include "objects.h"
#include "environment.h"

class Evaluator {
public:
    Ad_Object* Eval(Ad_AST_Node*, Environment&);
    Ad_Object* EvalProgram(Ad_AST_Node*, Environment&);
    Ad_Object* EvalInfixExpression(std::string, Ad_Object*, Ad_Object*);
    Ad_Object* EvalIntegerInfixExpression(std::string, Ad_Object*, Ad_Object*);
    Ad_Object* EvalPrefixExpression(std::string, Ad_Object*);
    Ad_Object* NewError(std::string);
    Ad_Object* EvalBangOperatorExpression(Ad_Object*);
    Ad_Object* EvalMinusPrefixOperatorExpression(Ad_Object*);
    Ad_Object* EvalIdentifier(Ad_AST_Node*, Environment&);
    Ad_Object* EvalIfExpression(Ad_AST_Node*, Environment&);
};

#endif