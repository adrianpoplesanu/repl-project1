from token_type import TokenType

class PrecedenceType(object):
    LOWEST = 1
    EQUALS = 2
    CONDITIONAL = 3
    LESSGREATER = 4
    SUM = 5
    PRODUCT = 6
    PREFIX = 7
    CALL = 8
    INDEX = 9

precedences = {
    TokenType.EQ: PrecedenceType.EQUALS,
    TokenType.NOT_EQ: PrecedenceType.EQUALS,
    TokenType.LT: PrecedenceType.LESSGREATER,
    TokenType.GT: PrecedenceType.LESSGREATER,
    TokenType.LTE: PrecedenceType.LESSGREATER,
    TokenType.GTE: PrecedenceType.LESSGREATER,
    TokenType.AND: PrecedenceType.CONDITIONAL,
    TokenType.OR: PrecedenceType.CONDITIONAL,
    TokenType.PLUS: PrecedenceType.SUM,
    TokenType.MINUS: PrecedenceType.SUM,
    TokenType.SLASH: PrecedenceType.PRODUCT,
    TokenType.ASTERISK: PrecedenceType.PRODUCT,
    TokenType.LPAREN: PrecedenceType.CALL,
    TokenType.LBRACKET: PrecedenceType.INDEX
}
