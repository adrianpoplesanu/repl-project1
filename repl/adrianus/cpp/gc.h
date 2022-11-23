#ifndef __GC_H
#define __GC_H

#include "environment.h"
#include "objects.h"
#include "ast.h"

class GarbageCollector {
public:
    std::vector<Environment*> gc_environments;
    std::vector<Ad_Object*> gc_objects;
    std::vector<Ad_AST_Node*> gc_ast_nodes;

    GarbageCollector();
    ~GarbageCollector();
    void addEnvironment(Environment*);
    void addObject(Ad_Object*);
    void addAstNode(Ad_AST_Node*);
    void clearEnvironments();
    void clearObjects();
    void clearAstNodes();
    void sweepEnvironments();
    void sweepObjects();
    void sweepAstNodes();
    void forceFreeEnvironments();
    void forceFreeObjects();
    void forceFreeAstNodes();
};

#endif