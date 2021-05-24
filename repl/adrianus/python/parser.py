from lexer import Lexer
from token_type import TokenType

class Parser(object):
	def __init__(self):
		self.lexer = Lexer()
		self.source = ''
		self.current_token = None
		self.peek_token = None

	def reset(self, source):
		self.source = source
		self.lexer.load(source)
		self.next_token()
		self.next_token()

	def build_program_statements(self, program):
		while self.current_token.type != TokenType.EOF:
			print str(self.current_token)
			self.next_token()

	def next_token(self):
		self.current_token = self.peek_token
		self.peek_token = self.lexer.next_token()