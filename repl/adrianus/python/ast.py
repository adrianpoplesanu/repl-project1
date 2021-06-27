class StatementType(object):
    PROGRAM = 'PROGRAM'
    LET_STATEMENT = 'LET_STATEMENT'
    RETURN_STATEMENT = 'RETURN_STATEMENT'
    EXPRESSION_STATEMENT = 'EXPRESSION_STATEMENT'
    IDENTIFIER = 'IDENTIFIER'
    INTEGER = 'INTEGER'
    BOOLEAN = 'BOOLEAN'
    INFIX_EXPRESSION = 'INFIX_EXPRESSION'
    PREFIX_EXPRESSION = 'PREFIX_EXPRESSION'
    CALL_EXPRESSION = 'CALL_EXPRESSION'
    IF_EXPRESSION = 'IF_EXPRESSION'
    BLOCK_STATEMENT = 'BLOCK_STATEMENT'
    FUNCTION_LITERAL = 'FUNCTION_LITERAL'
    WHILE_EXPRESSION = 'WHILE_EXPRESSION'


statement_type_map = {
    StatementType.PROGRAM: 'PROGRAM',
    StatementType.LET_STATEMENT: 'LET_STATEMENT',
    StatementType.RETURN_STATEMENT: 'RETURN_STATEMENT',
    StatementType.EXPRESSION_STATEMENT: 'EXPRESSION_STATEMENT',
    StatementType.IDENTIFIER: 'IDENTIFIER',
    StatementType.INTEGER: 'INTEGER',
    StatementType.BOOLEAN: 'BOOLEAN',
    StatementType.INFIX_EXPRESSION: 'INFIX_EXPRESSION',
    StatementType.PREFIX_EXPRESSION: 'PREFIX_EXPRESSION',
    StatementType.CALL_EXPRESSION: 'CALL_EXPRESSION',
    StatementType.IF_EXPRESSION: 'IF_EXPRESSION',
    StatementType.BLOCK_STATEMENT: 'BLOCK_STATEMENT',
    StatementType.FUNCTION_LITERAL: 'FUNCTION_LITERAL',
    StatementType.WHILE_EXPRESSION: 'WHILE_EXPRESSION'
}


class ASTNode(object):
    def __init__(self):
        pass

    def token_literal(self):
        print 'token_literal unimplemented in subclass'

    def __str__(self):
        print '__str__ not implmented in subclass'


class ASTProgram(ASTNode):
    def __init__(self):
        self.statements = []

    def reset(self):
        self.statements = []

    def debug(self):
        for statement in self.statements:
            print statement


class ASTBoolean(ASTNode):
    def __init__(self, token=None, value=None):
        """
        @param token: the node's token
        @param value: native bool value
        """
        self.token = token
        self.value = value

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return str(self.value)


class ASTIdentifier(ASTNode):
    def __init__(self, token=None, value=''):
        """
        @param token: the node's token
        @param value: string the name of the "variable"
        """
        self.token = token
        self.value = value

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'Identifier [token: ' + str(self.token) + ', value: ' + str(self.value) + ']'


class ASTInteger(ASTNode):
    def __init__(self, token=None, value=None):
        """
        @param token: the node's token
        @param value: native int value
        """
        self.token = token
        self.value = value

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        #print type(self.token.literal)
        #print type(self.value)
        return 'IntegerLiteral [' + str(self.token.literal) + '] <' + str(self.value) + '>'


class ASTLetStatement(ASTNode):
    def __init__(self, token=None, name=None, value=None):
        self.token = token
        self.name = name
        self.value = value

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'LetStatement [' + str(self.token.literal) + '] <' + str(self.name.value) +'>: ' + (str(self.value) if self.value else '')


class ASTReturnStatement(ASTNode):
    def __init__(self, token=None, value=None):
        self.token = token
        self.value = value

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'ReturnStatement [' + str(self.token.literal) + '] <' + (str(self.value) if self.value else '') + '>'


class ASTExpressionStatement(ASTNode):
    def __init__(self, token=None, expression=None):
        self.token = token
        self.expression = expression

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        #return 'TODO: implemement __str__ in ASTExpressionStatement'
        print type(self.expression)
        return str(self.expression)


class ASTIfExpression(ASTNode):
    def __init__(self, token=None, condition=None, consequence=None, alternative=None):
        """
        @param token: the node's token
        @param condition: expression???
        @param consequence: block statement
        @param alternative: block statement
        """
        self.token = token
        self.condition = condition
        self.consequence = consequence
        self.alternative = alternative

    def token_literal(self):
        return 'todo'

    def __str__(self):
        return 'TODO: implement __str__ in ASTIfExpression'


class ASTBlockStatement(ASTNode):
    def __init(self, token=None, statements=[]):
        self.token = token
        self.statements = statements

    def token_literal(self):
        return 'todo'

    def __str__(self):
        return 'TODO: implement __str__ in ASTBlockStatement'


class ASTFunctionLiteral(ASTNode):
    def __init__(self, token=None):
        self.token = token

    def token_literal(self):
        return 'todo'

    def __str__(self):
        return 'TODO: implement __str__ in ASTFunctionLiteral'


class ASTCallExpression(ASTNode):
    pass


class ASTPrefixExpression(ASTNode):
    def __init__(self, token=None, operator=None, right=None):
        """
        @param token: the node's token
        @param operator: string, the operator
        @param right: the node that gets "minused" or "banged"
        """
        self.token = token
        self.operator = operator
        self.right = right

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return '(' + self.operator + str(self.right) + ')'


class ASTInfixExpression(ASTNode):
    pass


class ASTWhileExpression(ASTNode):
    def __init__(self, token=None, condition=None, block=None):
        self.token = token
        self.condition = condition
        self.block = block

    def token_literal(self):
        return 'todo'

    def __str__(self):
        return 'TODO: implement __str__ in ASTWhileExpression'