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
	TokenType.START_COMMENT: "STRAT_COMMENT",
	TokenType.END_COMMENT: "END_COMMENT",
	TokenType.PLUSPLUS: 'INCREMENT_OPERATOR',
	TokenType.NULL: 'NULL'
}

keywords_map = {
	"let": TokenType.LET,
	"return": TokenType.RETURN,
	"break": TokenType.BREAK,
	"continue": TokenType.CONTINUE,
	"while": TokenType.WHILE,
	"for": TokenType.FOR,
	"function": TokenType.FUNCTION,
	"def": TokenType.DEF,
	"if": TokenType.IF,
	"else": TokenType.ELSE,
	"true": TokenType.TRUE,
	"false": TokenType.FALSE,
	"and": TokenType.AND,
	"or": TokenType.OR,
	"class": TokenType.CLASS,
	"this": TokenType.THIS,
	"null": TokenType.NULL
}


class Token(object):
	def __init__(self, type=None, literal=None):
		self.type = type
		self.literal = literal

	def __str__(self):
		return "Token[type: {0}, literal: {1}]".format(token_type_map[self.type], self.literal)