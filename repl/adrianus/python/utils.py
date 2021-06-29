from ast import StatementType

def print_ast_nodes(node, level):
    if not node:
        return
    print_level(level)
    if node.type == StatementType.PROGRAM:
        print 'Program'
        for statement in node.statements:
            print_ast_nodes(statement, level + 2)
    elif node.type == StatementType.LET_STATEMENT:
        print '->LetStatement ' + str(node.token) + ' ' + str(node.name.token)
        print_ast_nodes(node.value, level + 2)
    elif node.type == StatementType.RETURN_STATEMENT:
        print '->ReturnStatement ' + str(node.token)
        print_ast_nodes(node.value, level + 2)
    elif node.type == StatementType.EXPRESSION_STATEMENT:
        print '->ExpressionStatement ' + str(node.token)
        if node.expression:
            print_ast_nodes(node.expression, level + 2)
    elif node.type == StatementType.IDENTIFIER:
        print '->Identifier ' + str(node.token)
    elif node.type == StatementType.INTEGER:
        print '->Integer ' + str(node.token)
    elif node.type == StatementType.BOOLEAN:
        print '->Boolean ' + str(node.token)
    elif node.type == StatementType.INFIX_EXPRESSION:
        print '->InfixExpression ' + str(node.token)
        if node.left:
            print_ast_nodes(node.left, level + 2)
        if node.right:
            print_ast_nodes(node.right, level + 2)
    elif node.type == StatementType.PREFIX_EXPRESSION:
        print '->PrefixExpression ' + str(node.token)
        if node.right:
            print_ast_nodes(node.right, level + 2)
    elif node.type == StatementType.CALL_EXPRESSION:
        print '->CallExpression ' + str(node.token)
        print_ast_nodes(node.func, level + 2)
        for argument in node.arguments:
            print_ast_nodes(argument, level + 2)
    elif node.type == StatementType.IF_EXPRESSION:
        print '->IfExpression ' + str(node.token)
        print_ast_nodes(node.condition, level + 2)
        print_ast_nodes(node.consequence, level + 2)
        print_ast_nodes(node.alternative, level + 2)
    elif node.type == StatementType.BLOCK_STATEMENT:
        print '->BlockStatement ' + str(node.token)
        for statement in node.statements:
            print_ast_nodes(statement, level + 2)
    elif node.type == StatementType.FUNCTION_LITERAL:
        print '->FunctionLiteral ' + str(node.token)
        for parameter in node.parameters:
            print_ast_nodes(parameter, level + 2)
        print_ast_nodes(node.body, level + 2)
    elif node.type == StatementType.WHILE_EXPRESSION:
        print '->WhileExpression ' + str(node.token)
        print_ast_nodes(node.condition, level + 2)
        print_ast_nodes(node.consequence, level + 2)
    else:
        print 'ERROR: print ast node unknow type'

def print_level(level):
    if level:
        print ' ' * (level - 1),