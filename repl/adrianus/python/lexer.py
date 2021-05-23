from token import TokenType


class Lexer(object):
	def __init__(self, position=0, read_position=0, source=''):
		self.position = position
		self.read_position = read_position
		self.ch = ''
		self.source = source

	def load(self, source):
		self.position = 0
		self.readPosition = 0
		self.ch = ''
		self.source = source

	def next_token(self):
		pass

	def read_char(self):
		if read_position < len(self.source):
			self.ch = self.source[read_position]
		else:
			self.ch = 0
		position = read_position
		read_position += 1

	def skip_whitespaces(self):
		while self.ch == ' ' or self.ch == '\t' or self.ch == '\r' or self.ch == '\n':
			read_char()

	def is_letter(self):
		return ('a' <= self.ch and self.ch <= 'z') or ('A' <= self.ch and self.ch <= 'Z') or sel.ch == '_'

	def is_digit(self):
		pass

	def lookup_ident(self):
		pass

	def peek_char(self):
		pass

	def read_ident(self):
		pass

	def read_number(self):
		pass