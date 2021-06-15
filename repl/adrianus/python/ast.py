class StatementType(object):
    PROGRAM = 'PROGRAM'
    LET_STATEMENT = 'LET_STATEMENT'
    RETURN_STATEMENT = 'RETURN_STATEMENT'
    EXPRESSION_STATEMENT = 'EXPRESSION_STATEMENT'
    IDENTIFIER = 'IDENTIFIER'
    INTEGER = 'INTEGER'
    BOOLEAN = 'BOOLEAN'
    STATEMENT = 'STATEMENT'
    EXPRESSION = 'EXPRESSION'
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
    StatementType.STATEMENT: 'STATEMENT',
    StatementType.EXPRESSION: 'EXPRESSION',
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


class ASTInteger(ASTNode):
	pass


class ASTBoolean(ASTNode):
	pass


class ASTProgram(ASTNode):
	def __init__(self):
		self.statements = []

	def reset(self):
		self.statements = []

	def debug(self):
		for statement in self.statements:
			print statement


class ASTIdentifier(ASTNode):
	def __init__(self, token=None, value=''):
		self.token = token
		self.value = value

	def token_literal(self):
		return self.token.literal

	def __str__(self):
		return 'Identifier [token: ' + str(self.token) + ', value: ' + str(self.value) + ']'


class ASTInteger(ASTNode):
	pass


class ASTBoolean(ASTNode):
	pass


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
	pass