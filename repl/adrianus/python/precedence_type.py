from token_type import TokenType

class PrecedenceType(object):
    LOWEST = 1
    ASSIGN = 2
    EQUALS = 3
    CONDITIONAL = 4
    LESSGREATER = 5
    SUM = 6
    PRODUCT = 7
    PLUSPLUS = 8
    PREFIX = 9
    CALL = 10
    INDEX = 11
    MEMBER_ACCESS = 12

precedences = {
    TokenType.ASSIGN: PrecedenceType.ASSIGN,
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
    TokenType.LBRACKET: PrecedenceType.INDEX,
    TokenType.DOT: PrecedenceType.MEMBER_ACCESS,
    TokenType.PLUSPLUS: PrecedenceType.PLUSPLUS
}
