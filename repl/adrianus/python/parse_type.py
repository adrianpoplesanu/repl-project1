from token_type import TokenType

class ParseType(object):
    LOWEST = 1
    EQUALS = 2
    LESSGREATER = 3
    SUM = 4
    PRODUCT = 5
    PREFIX = 6
    CALL = 7

precedences = {
    TokenType.EQ: ParseType.EQUALS,
    TokenType.NOT_EQ: ParseType.EQUALS,
    TokenType.LT: ParseType.LESSGREATER,
    TokenType.GT: ParseType.LESSGREATER,
    TokenType.PLUS: ParseType.SUM,
    TokenType.MINUS: ParseType.SUM,
    TokenType.SLASH: ParseType.PRODUCT,
    TokenType.ASTERISK: ParseType.PRODUCT,
    TokenType.LPAREN: ParseType.CALL
}
