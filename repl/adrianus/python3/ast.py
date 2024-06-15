class StatementType(object):
    PROGRAM = 'PROGRAM'
    LET_STATEMENT = 'LET_STATEMENT'
    RETURN_STATEMENT = 'RETURN_STATEMENT'
    EXPRESSION_STATEMENT = 'EXPRESSION_STATEMENT'
    IDENTIFIER = 'IDENTIFIER'
    INTEGER = 'INTEGER'
    FLOAT = 'FLOAT'
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
    ASSIGN_STATEMENT = 'ASSIGN_STATEMENT'
    DEF_STATEMENT = 'DEF_STATEMENT'
    CLASS_STATEMENT = 'CLASS_STATEMENT'
    MEMBER_ACCESS = 'MEMBER_ACCESS'
    COMMENT = 'COMMENT'
    PREFIX_INCREMENT = 'PREFIX_INCREMENT'
    POSTFIX_INCREMENT = 'POSTFIX_INCREMENT'
    FOR_EXPRESSION = 'FOR_EXPRESSION'
    NULL_EXPRESSION = 'NULL_EXPRESSION'
    THIS_EXPRESSION = 'THIS_EXPRESSION'
    SUPER_EXPRESSION = 'SUPER_EXPRESSION'
    BREAK_STATEMENT = 'BREAK_STATEMENT'
    CONTINUE_STATEMENT = 'CONTINUE_STATEMENT'
    PLUS_EQUALS_STATEMENT = 'ASSIGN_EQUALS_STATEMNT'


statement_type_map = {
    StatementType.PROGRAM: 'PROGRAM',
    StatementType.LET_STATEMENT: 'LET_STATEMENT',
    StatementType.RETURN_STATEMENT: 'RETURN_STATEMENT',
    StatementType.EXPRESSION_STATEMENT: 'EXPRESSION_STATEMENT',
    StatementType.IDENTIFIER: 'IDENTIFIER',
    StatementType.INTEGER: 'INTEGER',
    StatementType.FLOAT: 'FLOAT',
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
    StatementType.HASH_LITERAL: 'HASH_LITERAL',
    StatementType.ASSIGN_STATEMENT: 'ASSIGN_STATEMENT',
    StatementType.DEF_STATEMENT: 'DEF_STATEMENT',
    StatementType.CLASS_STATEMENT: 'CLASS_STATEMENT',
    StatementType.MEMBER_ACCESS: 'MEMBER_ACCESS',
    StatementType.COMMENT: 'COMMENT',
    StatementType.PREFIX_INCREMENT: 'PREFIX_INCREMENT',
    StatementType.POSTFIX_INCREMENT: 'POSTFIX_INCREMENT',
    StatementType.FOR_EXPRESSION: 'FOR_EXPRESSION',
    StatementType.NULL_EXPRESSION: 'NULL_EXPRESSION',
    StatementType.THIS_EXPRESSION: 'THIS_EXPRESSION',
    StatementType.SUPER_EXPRESSION: 'SUPER_EXPRESSION',
    StatementType.BREAK_STATEMENT: 'BREAK_STATEMENT',
    StatementType.CONTINUE_STATEMENT: 'CONTINUE_STATEMENT'
}


class ASTNode(object):
    def __init__(self):
        pass

    def token_literal(self):
        print ('token_literal unimplemented in subclass')

    def __str__(self):
        print ('__str__ not implmented in subclass')


class ASTProgram(ASTNode):
    type = StatementType.PROGRAM

    def __init__(self):
        self.statements = []

    def reset(self):
        self.statements = []

    def debug(self):
        for statement in self.statements:
            print (statement)

    def __str__(self):
        return "ASTProgram"


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


class ASTAssignStatement(ASTNode):
    type = StatementType.ASSIGN_STATEMENT

    def __init__(self, token=None, name=None, value=None):
        self.token = token
        self.name = name
        self.value = value

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'AssignStatement [' + str(self.token.literal) + '] <' + str(self.name.value) +'>: ' + (str(self.value) if self.value else '')


class ASTPlusEqualsStatement(ASTNode):
    type = StatementType.PLUS_EQUALS_STATEMENT

    def __init__(self, token=None, name=None, value=None):
        self.token = token
        self.name = name
        self.value = value

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'PlusEqualsStatement [' + str(self.token.literal) + '] <' + str(self.name.value) + ">: " + (str(self.value) if self.value else '')


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
        print (type(self.expression))
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


class ASTFloat(ASTNode):
    type = StatementType.FLOAT

    def __init__(self, token=None, value=None):
        """
        @param token: the node's token
        @param value: native float value
        """
        self.token = token
        self.value = value

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'FloatLiteral [' + str(self.token.literal) + '] <' + str(self.value) + '>'


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
        @:param default_params: map of params
        """
        self.token = token
        self.func = func
        self.arguments = None
        self.kw_args = None

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
        out = ""
        for statement in self.statements:
            out += str(statement)
        return out


class ASTFunctionLiteral(ASTNode):
    type = StatementType.FUNCTION_LITERAL

    def __init__(self, token=None, parameters=None, default_params=None, body=None):
        """
        @param token: the node's token
        @param parameters: list
        @:param default_params: list of default params
        @param body: ASTBlockStatement
        """
        self.token = token
        self.parameters = parameters
        self.default_params = default_params
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
        @param condition: expression???
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


class ASTDefStatement(ASTNode):
    type = StatementType.DEF_STATEMENT

    def __init__(self, token=None, name=None, parameters=None, default_params=None, body=None):
        """
        @param name: AstNode - identifier that will store the function, it needs to be like a let statement in the eval
        @param params: AstNode[] list of funtion arguments
        @:param default_params: default params when missing argument
        @param body: AstNode block statement that is the body of the function
        """
        self.name = name
        self.token = token
        self.parameters = parameters
        self.default_params = default_params
        self.body = body

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'Token: ' + str(self.token) + '<name: ' + str(self.name) + ', params:' + str(self.parameters) + ', body: ' + str(self.body) + '>'


class ASTComment(ASTNode):
    type = StatementType.COMMENT

    def __init__(self, token=None):
        """
        @param token: the node's token
        no need for other params, this just has to be ingnored by the evaluator
        """
        self.token = token

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'ASTComment'


class ASTClassStatement(ASTNode):
    type = StatementType.CLASS_STATEMENT

    def __init__(self, token=None, name=None, methods=None, attributes=None, inherit_from=None):
        """
        @param token: the node's token
        @param name: ASTIdentifier, name of the class
        @param methods: list of ASTDefStatement
        @param attributes: list of ASTAssignStatement
        @param inherit_from: list of parents the class inherits from
        """
        self.token = token
        self.name = name
        self.methods = methods
        self.attributes = attributes
        self.inherit_from = inherit_from

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'Token: ' + str(self.token)


class ASTMemberAccess(ASTNode):
    type = StatementType.MEMBER_ACCESS

    def __init__(self, token=None, arguments=None, owner=None, member=None, is_method=False):
        """
        @param token: the node's token
        @param arguments:
        @param owner:
        @param member:
        @param is_method: boolean, indicates is this member access is a method call or an attribute access
        """
        self.token = token
        self.arguments = arguments
        self.owner = owner
        self.member = member
        self.is_method = is_method

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'Token: ' + str(self.token)


class ASTPrefixIncrement(ASTNode):
    type = StatementType.PREFIX_INCREMENT

    def __init__(self, token=None, name=None, operator=None):
        """
        @param token: the node's token
        @param name: AstNode, Identifier that is the target of the ++ operation
        """
        self.token = token
        self.name = name
        self.operator = operator

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'Token: ' + str(self.token)


class ASTPostfixIncrement(ASTNode):
    type = StatementType.POSTFIX_INCREMENT

    def __init__(self, token=None, name=None, operator=None):
        """
        @param token: the node's token
        """
        self.token = token
        self.name = name
        self.operator = operator

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'Token: ' + str(self.token)


class ASTForExpression(ASTNode):
    type = StatementType.FOR_EXPRESSION

    def __init__(self, token=None, initialization=None, condition=None, step=None, body=None):
        """
        @param token: the node's token
        """
        self.token = token
        self.initialization = initialization
        self.condition = condition
        self.step = step
        self.body = body

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'Token: ' + str(self.token)


class ASTNullExpression(ASTNode):
    type = StatementType.NULL_EXPRESSION

    def __init__(self, token=None):
        """
        @param token: the node's token
        """
        self.token = token

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'Token: ' + str(self.token)


class ASTThisExpression(ASTNode):
    type = StatementType.THIS_EXPRESSION

    def __init__(self, token=None):
        """
        @param token: the node's token
        """
        self.token = token

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'Token: ' + str(self.token)


class ASTSuperExpression(ASTNode):
    type = StatementType.SUPER_EXPRESSION

    def __init__(self, token=None):
        """
        @param token: the node's token
        """
        self.token = token

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'Token: ' + str(self.token)


class ASTBrakeStatement(ASTNode):
    type = StatementType.BREAK_STATEMENT

    def __init__(self, token=None):
        """
        @param token: the node's token
        """
        self.token = token

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'Token: ' + str(self.token)


class ASTContinueStatement(ASTNode):
    type = StatementType.CONTINUE_STATEMENT

    def __init__(self, token=None):
        """
        @param token: the node's token
        """
        self.token = token

    def token_literal(self):
        return self.token.literal

    def __str__(self):
        return 'Token: ' + str(self.token)
