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
	TokenType.FUNC: "FUNC",
	TokenType.DEF: "DEF",
	TokenType.FUN: "FUN",
	TokenType.FUNCTION: "FUNCTION",
	TokenType.IDENT: "IDENT",
	TokenType.INT: "INT",
	TokenType.PLUS: "PLUS",
	TokenType.MINUS: "MINUS",
	TokenType.SLASH: "SLASH",
	TokenType.ASTERISK: "ASTERISK",
	TokenType.MODULUS: "MODULUS",
	TokenType.BANG: "BANG",
	TokenType.COMMA: "COMMA",
	TokenType.SEMICOLON: "SEMICOLON",
	TokenType.LPAREN: "LPAREN",
	TokenType.RPAREN: "RPAREN",
	TokenType.LBRACE: "LBRACE",
	TokenType.RBRACE: "RBRACE",
	TokenType.LT: "LT",
	TokenType.GT: "GT",
	TokenType.LTE: "LTE",
	TokenType.GTE: "GTE",
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
	TokenType.START_COMMENT: "START_COMMENT",
	TokenType.MULTI_COMMENT: "MULTI_COMMENT",
	TokenType.END_COMMENT: "END_COMMENT",
	TokenType.PLUSPLUS: 'INCREMENT_OPERATOR',
	TokenType.MINUSMINUS: 'DECREMENT_OPERATOR',
	TokenType.PLUS_EQ: 'PLUS_EQ',
	TokenType.MINUS_EQ: 'MINUS_EQ',
	TokenType.NULL: 'NULL',
	TokenType.SUPER: 'SUPER'
}

keywords_map = {
	"let": TokenType.LET,
	"return": TokenType.RETURN,
	"func": TokenType.FUNC,
	"if": TokenType.IF,
	"else": TokenType.ELSE,
	"true": TokenType.TRUE,
	"false": TokenType.FALSE,
	"while": TokenType.WHILE,
	"def": TokenType.DEF,
	"fun": TokenType.FUN,
	"function": TokenType.FUNCTION,
	"null": TokenType.NULL,
	"class": TokenType.CLASS,
	"this": TokenType.THIS,
	"and": TokenType.AND,
	"or": TokenType.OR,
	"for": TokenType.FOR,
	"break": TokenType.BREAK,
	"continue": TokenType.CONTINUE,
	"super": TokenType.SUPER
}


class Token(object):
	def __init__(self, type=None, literal=None):
		self.type = type
		self.literal = literal

	def __str__(self):
		return "Token[type: {0}, literal: {1}]".format(token_type_map[self.type], self.literal)
