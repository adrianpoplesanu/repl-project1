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
    STRING_LITERAL = 'STRING_LITERAL'
    LIST_LITERAL = 'LIST_LITERAL'
    INDEX_EXPRESSION = 'INDEX_EXPRESSION'
    HASH_LITERAL = 'HASH_LITERAL'


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
    StatementType.WHILE_EXPRESSION: 'WHILE_EXPRESSION',
    StatementType.STRING_LITERAL: 'STRING_LITERAL',
    StatementType.LIST_LITERAL: 'LIST_LITERAL',
    StatementType.INDEX_EXPRESSION: 'INDEX_EXPRESSION',
    StatementType.HASH_LITERAL: 'HASH_LITERAL'
}


class ASTNode(object):
    def __init__(self):
        pass

    def token_literal(self):
        print 'token_literal unimplemented in subclass'

    def __str__(self):
        print '__str__ not implmented in subclass'


class ASTProgram(ASTNode):
    type = StatementType.PROGRAM

    def __init__(self):
        self.statements = []

    def reset(self):
        self.statements = []

    def debug(self):
        for statement in self.statements:
            print statement


class ASTLetStatement(ASTNode):
    type = StatementType.LET_STATEMENT

    def __init__(self, token=None, name=None, value=None):
        self.token = token
        self.name = name
        self.value = value

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'LetStatement [' + str(self.token.literal) + '] <' + str(self.name.value) +'>: ' + (str(self.value) if self.value else '')


class ASTReturnStatement(ASTNode):
    type = StatementType.RETURN_STATEMENT

    def __init__(self, token=None, value=None):
        self.token = token
        self.value = value

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'ReturnStatement [' + str(self.token.literal) + '] <' + (str(self.value) if self.value else '') + '>'


class ASTExpressionStatement(ASTNode):
    type = StatementType.EXPRESSION_STATEMENT

    def __init__(self, token=None, expression=None):
        self.token = token
        self.expression = expression

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        #return 'TODO: implemement __str__ in ASTExpressionStatement'
        print type(self.expression)
        return str(self.expression)


class ASTIdentifier(ASTNode):
    type = StatementType.IDENTIFIER

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
    type = StatementType.INTEGER

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


class ASTBoolean(ASTNode):
    type = StatementType.BOOLEAN

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


class ASTInfixExpression(ASTNode):
    type = StatementType.INFIX_EXPRESSION

    def __init__(self, token=None, operator=None, left=None, right=None):
        """
        @param token: the node's token
        @param operator: string the infix operator
        @param left: ASTNode subclass, the left operand
        @param right: ASTNode subclass, the right operand
        """
        self.token = token
        self.operator = operator
        self.left = left
        self.right = right

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return str(self.left) + ' ' + str(self.operator) + ' ' + str(self.right)


class ASTPrefixExpression(ASTNode):
    type = StatementType.PREFIX_EXPRESSION

    def __init__(self, token=None, operator=None, right=None):
        """
        @param token: the node's token
        @param operator: string, the operator
        @param right: the node(ASTNode subclass) that gets "minused" or "banged"
        """
        self.token = token
        self.operator = operator
        self.right = right

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return '(' + self.operator + str(self.right) + ')'


class ASTCallExpression(ASTNode):
    type = StatementType.CALL_EXPRESSION

    def __init__(self, token=None, func=None):
        """
        @param token: the node's token
        @param func: ASTNode subclass representing the Function Literal
        arguments: the list of arguments in the call
        """
        self.token = token
        self.func = func
        self.arguments = None

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return str(self.func) + '(some arguments here)'


class ASTIfExpression(ASTNode):
    type = StatementType.IF_EXPRESSION

    def __init__(self, token=None, condition=None, consequence=None, alternative=None):
        """
        @param token: the node's token
        @param condition: expression???
        @param consequence: ASTBlockStatement
        @param alternative: ASTBlockStatement
        """
        self.token = token
        self.condition = condition
        self.consequence = consequence
        self.alternative = alternative

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'TODO: implement __str__ in ASTIfExpression'


class ASTBlockStatement(ASTNode):
    type = StatementType.BLOCK_STATEMENT

    def __init__(self, token=None, statements=None):
        """
        @param token: the node's token
        @param statements: list
        """
        self.token = token
        if statements:
            self.statements = statements
        else:
            self.statements = []

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'TODO: implement __str__ in ASTBlockStatement'


class ASTFunctionLiteral(ASTNode):
    type = StatementType.FUNCTION_LITERAL

    def __init__(self, token=None, parameters=None, body=None):
        """
        @param token: the node's token
        @param parameters: list
        @param body: ASTBlockStatement
        """
        self.token = token
        self.parameters = parameters
        self.body = body

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'TODO: implement __str__ in ASTFunctionLiteral'


class ASTWhileExpression(ASTNode):
    type = StatementType.WHILE_EXPRESSION

    def __init__(self, token=None, condition=None, block=None):
        """
        @param token: the node's token
        @param condition: ???
        @param block: ASTBlockStatement
        """
        self.token = token
        self.condition = condition
        self.block = block

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'TODO: implement __str__ in ASTWhileExpression'


class ASTStringLiteral(ASTNode):
    type = StatementType.STRING_LITERAL

    def __init__(self, token=None, value=None):
        """
        @param token: the node's token
        @param value: string
        """
        self.token = token
        self.value = value

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'TODO: implement __str__ in ASTStringLiteral'


class ASTListLiteral(ASTNode):
    type = StatementType.LIST_LITERAL

    def __init__(self, token=None, elements=None):
        """
        @param token: the node's token
        @param elements: the list of Ad objects
        """
        self.token = token
        self.elements = elements

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'TODO: implement __str__ in ASTListLiteral'


class ASTIndexExpression(ASTNode):
    type = StatementType.INDEX_EXPRESSION

    def __init__(self, token=None, left=None, index=None):
        """
        @param token: the node's token
        @param left: ASTListLiteral
        @param index: int, which element is indexed
        """
        self.token = token
        self.left = left
        self.index = index

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'TODO: implement __str__ in ASTIndexExpression'


class ASTHashLiteral(ASTNode):
    type = StatementType.HASH_LITERAL

    def __init__(self, token=None, pairs=None):
        """
        @param token: the node's token
        @param pairs: map [Expression] => Expression
        """
        self.token = token
        self.pairs = pairs

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'TODO: implement __str__ in ASTHashLiteral'