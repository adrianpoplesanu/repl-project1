#ifndef __EVALUATOR_H
#define __EVALUATOR_H

#include "ast.h"
#include "objects.h"
#include "environment.h"
#include "settings.h"
#include "gc.h"
#include <unordered_map>
#include <ctime>

class Evaluator {
public:
    std::vector<Environment*> environment_garbage_collection;
    GarbageCollector *garbageCollector;
    std::unordered_map<StatementType, double> eval_times_per_statement_type;
    Ad_Object *tmp_obj;

    Ad_Object* Eval(Ad_AST_Node*, Environment&);
    Ad_Object* EvalProgram(Ad_AST_Node*, Environment&);
    Ad_Object* EvalInfixExpression(std::string, Ad_Object*, Ad_Object*);
    Ad_Object* EvalIntegerInfixExpression(std::string, Ad_Object*, Ad_Object*);
    Ad_Object* EvalBooleanInfixExpression(std::string, Ad_Object*, Ad_Object*);
    Ad_Object* EvalStringAndIntInfixExpression(std::string, Ad_Object*, Ad_Object*);
    Ad_Object* EvalIntAndStringInfixExpression(std::string, Ad_Object*, Ad_Object*);
    Ad_Object* EvalFloatInfixExpression(std::string, Ad_Object*, Ad_Object*);
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
    Ad_Object* ApplyFunction(Ad_Object*, std::vector<Ad_Object*>, Environment&);
    Ad_Object* CallInstanceConstructor(Ad_Object*, std::vector<Ad_Object*>, Environment&);
    Environment ExtendFunctionEnv(Ad_Object*, std::vector<Ad_Object*>);
    Environment* extendFunctionEnv(Ad_Object*, std::vector<Ad_Object*>);
    Ad_Object* UnwrapReturnValue(Ad_Object*, Environment*);
    Ad_Object* EvalWhileExpression(Ad_AST_Node*, Environment&);
    Ad_Object* EvalString(Ad_AST_Node*, Environment&);
    Ad_Object* EvalStringInfixExpression(std::string, Ad_Object*, Ad_Object*);
    //Ad_Object* EvalIndexExpression(Ad_Object*, Ad_Object*);
    Ad_Object* evalCallExpression(Ad_AST_Node*, Environment*);
    Ad_Object* evalIndexExpression(Ad_AST_Node*, Environment*);
    Ad_Object* EvalListIndexExpression(Ad_Object*, Ad_Object*);
    Ad_Object* EvalHashLiteral(Ad_AST_Node*, Environment&);
    Ad_Object* EvalHashIndexExpression(Ad_Object*, Ad_Object*);
    Ad_Object* evalStringIndexExpression(Ad_Object*, Ad_Object*);
    Ad_Object* EvalAssignStatement(Ad_AST_Node*, Environment&);
    Ad_Object* EvalIndexExpressionAssign(Ad_AST_Node*, Environment&);
    Ad_Object* EvalDefStatement(Ad_AST_Node*, Environment&);
    Ad_Object* EvalClassStatement(Ad_AST_Node*, Environment&);
    void updateInstanceWithInheritedClasses(Ad_Object*, Environment&);
    Ad_Object* EvalMemberAccess(Ad_AST_Node*, Environment&);
    Ad_Object* evalRecursiveMemberAccessCall(Ad_AST_Node*, Environment&);
    Ad_Object* recursiveMemberAccessAssign(Ad_AST_Node*, Environment*);
    Ad_Object* EvalFileObjectMethod(Ad_AST_Node*, std::vector<Ad_AST_Node*>, Environment&);
    Ad_Object* evalSocketObjectMethod(Ad_AST_Node*, std::vector<Ad_AST_Node*>, Environment&);
    Ad_Object* evalThreadObjectMethod(Ad_AST_Node*, std::vector<Ad_AST_Node*>, Environment&);
    Ad_Object* ApplyMethod(Ad_Object*, std::vector<Ad_Object*>, Environment&);
    Ad_Object* EvalPrefixIncrement(Ad_AST_Node*, Environment&);
    Ad_Object* EvalPostfixIncrement(Ad_AST_Node*, Environment&);
    Ad_Object* EvalForExpression(Ad_AST_Node*, Environment&);
    Ad_Object* EvalBreakStatement(Ad_AST_Node*, Environment&);
    Ad_Object* EvalContinueStatement(Ad_AST_Node*, Environment&);
    Ad_Object* evalPlusEqualsStatement(Ad_AST_Node*, Environment*);
    Ad_Object* evalPlusEqualsIndexExpression(Ad_AST_Node*, Environment*);
    Ad_Object* EvalNullExpression(Ad_AST_Node*, Environment&);
    Ad_Object* EvalFloatExpression(Ad_AST_Node*, Environment&);
    Ad_Object* evalThisExpression(Ad_AST_Node*, Environment*);
    Ad_Object* newSubList(Ad_Object*, int, int, int);
    Ad_Object* evalSubListIndexExpression(Ad_Object*, Ad_Object*, Ad_Object*, Ad_Object*);
    Ad_Object* evalSubListIndexExpressionWithIndexStartMissing(Ad_Object*, Ad_Object*, Ad_Object*, Ad_Object*);
    Ad_Object* evalSubListIndexExpressionWithIndexEndMissing(Ad_Object*, Ad_Object*, Ad_Object*, Ad_Object*);
    Ad_Object* evalSubListIndexExpressionWithIndexStartAndIndexEndMissing(Ad_Object*, Ad_Object*, Ad_Object*, Ad_Object*);
    Ad_Object* evalSubListIndexExpressionWithAllMissing(Ad_Object*, Ad_Object*, Ad_Object*, Ad_Object*);
    Ad_Object* newSubString(Ad_Object*, int, int, int);
    Ad_Object* evalSubStringIndexExpression(Ad_Object*, Ad_Object*, Ad_Object*, Ad_Object*);
    Ad_Object* evalSubStringIndexExpressionWithIndexStartMissing(Ad_Object*, Ad_Object*, Ad_Object*, Ad_Object*);
    Ad_Object* evalSubStringIndexExpressionWithIndexEndMissing(Ad_Object*, Ad_Object*, Ad_Object*, Ad_Object*);
    Ad_Object* evalSubStringIndexExpressionWithIndexAndIndexEndMissing(Ad_Object*, Ad_Object*, Ad_Object*, Ad_Object*);
    Ad_Object* evalSubStringIndexExpressionWithAllMissing(Ad_Object*, Ad_Object*, Ad_Object*, Ad_Object*);
    Environment* ExtendMethodEnv(Ad_Object*, std::vector<Ad_Object*>, Environment&);
    //void ExtendMethodEnv(Ad_Object*, std::vector<Ad_Object*>, Environment&);
    void Init();
    void GarbageCollectEnvironments();
    void addEnvironmentGarbageCollectorListener(Environment *);
    void setGarbageCollector(GarbageCollector*); // using this in eval builtin

    void initRuntimeStatistics();
    void printRuntimeStatistics();

private:
    bool validateNumberOfArguments(std::vector<int>, int);
};

void dispatchMemberAccessPerObjectType(Ad_Object*, Ad_AST_Identifier*, std::vector<Ad_Object*>);

#endif