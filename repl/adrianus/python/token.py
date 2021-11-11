from token_type import TokenType

token_type_map = {
	TokenType.ILLEGAL: "ILLEGAL",
	TokenType.ASSIGN: "ASSIGN",
	TokenType.EOF: "EOF",
	TokenType.LET: "LET",
	TokenType.RETURN: "RETURN",
	TokenType.BREAK: "BREAK",
	TokenType.CONTINUE: "CONTINUE",
	TokenType.WHILE: "WHILE",
	TokenType.FOR: "FOR",
	TokenType.FOREACH: "FOREACH",
	TokenType.FUNCTION: "FUNCTION",
	TokenType.DEF: "DEF",
	TokenType.IDENT: "IDENT",
	TokenType.INT: "INT",
	TokenType.PLUS: "PLUS",
	TokenType.MINUS: "MINUS",
	TokenType.SLASH: "SLASH",
	TokenType.ASTERISK: "ASTERISK",
	TokenType.BANG: "BANG",
	TokenType.COMMA: "COMMA",
	TokenType.SEMICOLON: "SEMICOLON",
	TokenType.LPAREN: "LPAREN",
	TokenType.RPAREN: "RPAREN",
	TokenType.LBRACE: "LBRACE",
	TokenType.RBRACE: "RBRACE",
	TokenType.LT: "LT",
	TokenType.GT: "GT",
	TokenType.EQ: "EQ",
	TokenType.NOT_EQ: "NOT_EQ",
	TokenType.IF: "IF",
	TokenType.ELSE: "ELSE",
	TokenType.TRUE: "TRUE",
	TokenType.FALSE: "FALSE",
	TokenType.STRING: "STRING",
	TokenType.AND: "AND",
	TokenType.OR: "OR",
	TokenType.LBRACKET: 'LBRACKET',
	TokenType.RBRACKET: 'RBRACKET',
	TokenType.COLON: "COLON",
	TokenType.DOT: "DOT",
	TokenType.CLASS: "CLASS",
	TokenType.THIS: "THIS",
	TokenType.START_COMMENT: "STRAT_COMMENT",
	TokenType.END_COMMENT: "END_COMMENT",
	TokenType.PLUSPLUS: 'PREFIX_INCREMENT'
}

keywords_map = {
	"let": TokenType.LET,
	"return": TokenType.RETURN,
	"if": TokenType.IF,
	"else": TokenType.ELSE,
	"function": TokenType.FUNCTION,
	"def": TokenType.DEF,
	"while": TokenType.WHILE,
	"true": TokenType.TRUE,
	"false": TokenType.FALSE,
	"for": TokenType.FOR,
	# context
	# "context": TokenType.ILLEGAL, context ar trebui sa fie un builtin
	"and": TokenType.AND,
	"or": TokenType.OR,
	"class": TokenType.CLASS,
	"this": TokenType.THIS
}


class Token(object):
	def __init__(self, type=None, literal=None):
		self.type = type
		self.literal = literal

	def __str__(self):
		return "Token[type: {0}, literal: {1}]".format(token_type_map[self.type], self.literal)