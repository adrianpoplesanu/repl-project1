from lexer import Lexer
from token_type import TokenType
from parse_type import ParseType, precedences
from ast import ASTLetStatement


class Parser(object):
	def __init__(self):
		self.lexer = Lexer()
		self.source = ''
		self.current_token = None
		self.peek_token = None
		self.prefix_parse_functions = {}
		self.infix_parse_functions = {}
		self.errors = []
		self.prefix_parse_functions[TokenType.IDENT] = self.parse_identifier
		self.prefix_parse_functions[TokenType.INT] = self.parse_integer_literal
		self.prefix_parse_functions[TokenType.BANG] = self.parse_prefix_expression
		self.prefix_parse_functions[TokenType.MINUS] = self.parse_prefix_expression
		self.prefix_parse_functions[TokenType.TRUE] = self.parse_boolean
		self.prefix_parse_functions[TokenType.FALSE] = self.parse_boolean
		self.prefix_parse_functions[TokenType.LPAREN] = self.parse_grouped_expression
		self.prefix_parse_functions[TokenType.IF] = self.parse_if_expression
		self.prefix_parse_functions[TokenType.FUNCTION] = self.parse_function_literal
		self.infix_parse_functions[TokenType.PLUS] = self.parse_infix_expression
		self.infix_parse_functions[TokenType.MINUS] = self.parse_infix_expression
		self.infix_parse_functions[TokenType.SLASH] = self.parse_infix_expression
		self.infix_parse_functions[TokenType.ASTERISK] = self.parse_infix_expression
		self.infix_parse_functions[TokenType.EQ] = self.parse_infix_expression
		self.infix_parse_functions[TokenType.NOT_EQ] = self.parse_infix_expression
		self.infix_parse_functions[TokenType.LT] = self.parse_infix_expression
		self.infix_parse_functions[TokenType.GT] = self.parse_infix_expression
		self.infix_parse_functions[TokenType.LPAREN] = self.parse_call_expression

	def reset(self, source):
		self.source = source
		self.lexer.load(source)
		self.next_token()
		self.next_token()

	def build_program_statements(self, program):
		while self.current_token.type != TokenType.EOF:
			print str(self.current_token)
			stmt = self.parse_statement()
			if stmt:
				program.statements.append(stmt)
			self.next_token()

	def next_token(self):
		self.current_token = self.peek_token
		self.peek_token = self.lexer.next_token()

	def current_token_is(self, token_type):
		return self.current_token.type == token_type

	def peek_token_is(self, token_type):
		return self.peek_token.type == token_type

	def expect_peek(self, token_type):
		if self.peek_token_is(token_type):
			self.next_token()
			return True
		else:
			self.errors.append('ERROR: i was expecting a different token here')
			return False

	def peek_precedence(self):
		if self.peek_token.type in precedences:
			return precedences[self.peek_token.type]
		return ParseType.LOWEST

	def current_precedence(self):
		if self.current_token.type in precedences:
			return precedences[self.current_token.type]
		return ParseType.LOWEST

	def parse_statement(self):
		if self.current_token.type == TokenType.LET:
			return self.parse_let_statement()
		elif self.current_token.type == TokenType.RETURN:
			return self.parse_return_statement()
		return self.parse_expression_statement()

	def parse_let_statement(self):
		return ASTLetStatement()

	def parse_return_statement(self):
		pass

	def parse_expression_statement(self):
		pass

	def parse_identifier(self):
		pass

	def parse_integer_literal(self):
		pass

	def parse_prefix_expression(self):
		pass

	def parse_boolean(self):
		pass

	def parse_grouped_expression(self):
		pass

	def parse_if_expression(self):
		pass

	def parse_function_literal(self):
		pass

	def parse_infix_expression(self):
		pass

	def parse_call_expression(self):
		pass