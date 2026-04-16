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
    Ad_Object* EvalInfixExpression(const std::string&, Ad_Object*, Ad_Object*);
    Ad_Object* EvalIntegerInfixExpression(std::string, Ad_Object*, Ad_Object*);
    Ad_Object* EvalBooleanInfixExpression(const std::string&, Ad_Object*, Ad_Object*);
    Ad_Object* EvalStringAndIntInfixExpression(const std::string&, Ad_Object*, Ad_Object*) const;
    Ad_Object* EvalIntAndStringInfixExpression(const std::string&, Ad_Object*, Ad_Object*) const;
    Ad_Object* EvalFloatInfixExpression(const std::string&, Ad_Object*, Ad_Object*);
    Ad_Object* EvalPrefixExpression(const std::string&, Ad_Object*);
    Ad_Object* NewError(std::string) const;
    static Ad_Object* EvalBangOperatorExpression(Ad_Object*);
    Ad_Object* EvalMinusPrefixOperatorExpression(Ad_Object*) const;
    Ad_Object* EvalIdentifier(Ad_AST_Node*, Environment&) const;
    Ad_Object* EvalIfExpression(Ad_AST_Node*, Environment&);
    Ad_Object* EvalBlockStatement(Ad_AST_Node*, Environment&);
    Ad_Object* EvalReturnStatement(Ad_AST_Node*, Environment&);
    static bool IsTruthy(Ad_Object*);
    static bool IsError(Ad_Object*);
    static Ad_Object* NativeBoolToBooleanObject(bool);
    std::vector<Ad_Object*> EvalExpressions(std::vector<Ad_AST_Node*>, Environment&);
    Ad_Object* ApplyFunction(Ad_Object*, const std::vector<Ad_Object*>&, Environment&);
    Ad_Object* CallInstanceConstructor(Ad_Object*, std::vector<Ad_Object*>, Environment&);
    Environment ExtendFunctionEnv(Ad_Object*, std::vector<Ad_Object*>);
    static Environment* extendFunctionEnv(Ad_Object*, std::vector<Ad_Object*>);
    static Ad_Object* UnwrapReturnValue(Ad_Object*, Environment*);
    Ad_Object* EvalWhileExpression(Ad_AST_Node*, Environment&);
    Ad_Object* EvalString(Ad_AST_Node*, Environment&) const;
    Ad_Object* EvalStringInfixExpression(const std::string&, Ad_Object*, Ad_Object*);
    //Ad_Object* EvalIndexExpression(Ad_Object*, Ad_Object*);
    Ad_Object* evalCallExpression(Ad_AST_Node*, Environment*);
    Ad_Object* evalIndexExpression(Ad_AST_Node*, Environment*);
    static Ad_Object* EvalListIndexExpression(Ad_Object*, Ad_Object*);
    Ad_Object* EvalHashLiteral(Ad_AST_Node*, Environment&);
    static Ad_Object* EvalHashIndexExpression(Ad_Object*, Ad_Object*);
    Ad_Object* evalStringIndexExpression(Ad_Object*, Ad_Object*) const;
    Ad_Object* EvalAssignStatement(Ad_AST_Node*, Environment&);
    Ad_Object* EvalIndexExpressionAssign(Ad_AST_Node*, Environment&);
    Ad_Object* EvalDefStatement(Ad_AST_Node*, Environment&) const;
    Ad_Object* EvalClassStatement(Ad_AST_Node*, Environment&) const;
    void updateInstanceWithInheritedClasses(Ad_Object*, Environment&);
    Ad_Object* EvalMemberAccess(Ad_AST_Node*, Environment&);
    Ad_Object* evalRecursiveMemberAccessCall(Ad_AST_Node*, Environment&);
    Ad_Object* recursiveMemberAccessAssign(Ad_AST_Node*, Environment*);
    Ad_Object* EvalFileObjectMethod(Ad_AST_Node*, const std::vector<Ad_AST_Node*>&, Environment&);
    Ad_Object* evalSocketObjectMethod(Ad_AST_Node*, std::vector<Ad_AST_Node*>, Environment&);
    Ad_Object* evalThreadObjectMethod(Ad_AST_Node*, std::vector<Ad_AST_Node*>, Environment&);
    Ad_Object* ApplyMethod(Ad_Object*, const std::vector<Ad_Object*>&, Environment&);
    Ad_Object* EvalPrefixIncrement(Ad_AST_Node*, Environment&) const;
    Ad_Object* EvalPostfixIncrement(Ad_AST_Node*, Environment&);
    Ad_Object* EvalForExpression(Ad_AST_Node*, Environment&);
    Ad_Object* EvalBreakStatement(Ad_AST_Node*, Environment&) const;
    Ad_Object* EvalContinueStatement(Ad_AST_Node*, Environment&) const;
    static Ad_Object* EvalNullExpression(Ad_AST_Node*, Environment&);
    Ad_Object* EvalFloatExpression(Ad_AST_Node*, Environment&) const;
    static Ad_Object* evalThisExpression(Ad_AST_Node*, Environment*);
    static Environment* ExtendMethodEnv(Ad_Object*, std::vector<Ad_Object*>, Environment&);
    //void ExtendMethodEnv(Ad_Object*, std::vector<Ad_Object*>, Environment&);
    static void Init();
    void GarbageCollectEnvironments() const;
    void addEnvironmentGarbageCollectorListener(Environment *);
    void setGarbageCollector(GarbageCollector*); // using this in eval builtin

    void initRuntimeStatistics();
    void printRuntimeStatistics();

private:
    static bool validateNumberOfArguments(std::vector<int>, int);
};

void dispatchMemberAccessPerObjectType(Ad_Object*, Ad_AST_Identifier*, std::vector<Ad_Object*>);

#endif