from ast import StatementType

def print_ast_nodes(node, level):
    if level:
        print ' ' * level,
    if (node.type == StatementType.PROGRAM):
        print 'Program'
    elif (node.type == StatementType.LET_STATEMENT):
        print '->LetStatement'
    elif (node.type == StatementType.RETURN_STATEMENT):
        print '->ReturnStatement'
    elif (node.type == StatementType.EXPRESSION_STATEMENT):
        print '->ExpressionStatement'
    elif (node.type == StatementType.IDENTIFIER):
        print '->Identifier'
    elif (node.type == StatementType.INTEGER):
        print '->Integer'
    elif (node.type == StatementType.BOOLEAN):
        print '->Boolean'
    elif (node.type == StatementType.INFIX_EXPRESSION):
        print '->InfixExpression'
    elif (node.type == StatementType.PREFIX_EXPRESSION):
        print '->PrefixExpression'
    elif (node.type == StatementType.CALL_EXPRESSION):
        print '->CallExpression'
    elif (node.type == StatementType.IF_EXPRESSION):
        print '->IfExpression'
    elif (node.type == StatementType.BLOCK_STATEMENT):
        print '->BlockStatement'
    elif (node.type == StatementType.FUNCTION_LITERAL):
        print '->FunctionLiteral'
    elif (node.type == StatementType.WHILE_EXPRESSION):
        print '->WhileExpression'
    else:
        print 'ERROR: print ast node unknow type'