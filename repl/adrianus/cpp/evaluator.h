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
    Ad_Object* EvalBlockStatement(Ad_AST_Node*, Environment&);
    Ad_Object* EvalReturnStatement(Ad_AST_Node*, Environment&);
    bool IsTruthy(Ad_Object*);
    bool IsError(Ad_Object*);
    Ad_Object* NativeBoolToBooleanObject(bool);
    std::vector<Ad_Object*> EvalExpressions(std::vector<Ad_AST_Node*>, Environment&);
    Ad_Object* ApplyFunction(Ad_Object*, std::vector<Ad_Object*>);
    Environment ExtendFunctionEnv(Ad_Object*, std::vector<Ad_Object*>);
    Ad_Object* UnwrapReturnValue(Ad_Object*);
    Ad_Object* EvalWhileExpression(Ad_AST_Node*, Environment&);
    Ad_Object* EvalString(Ad_AST_Node*, Environment&);
    Ad_Object* EvalStringInfixExpression(std::string, Ad_Object*, Ad_Object*);
    Ad_Object* EvalIndexExpression(Ad_Object*, Ad_Object*);
    Ad_Object* EvalListIndexExpression(Ad_Object*, Ad_Object*);
    Ad_Object* EvalHashLiteral(Ad_AST_Node*, Environment&);
    Ad_Object* EvalHashIndexExpression(Ad_Object*, Ad_Object*);
    Ad_Object* EvalAssignStatement(Ad_AST_Node*, Environment&);
    Ad_Object* EvalIndexExpressionAssign(Ad_AST_Node*, Environment&);
    Ad_Object* EvalDefStatement(Ad_AST_Node*, Environment&);
    Ad_Object* EvalClassStatement(Ad_AST_Node*, Environment&);
};

#endif