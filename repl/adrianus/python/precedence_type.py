from token_type import TokenType

class PrecedenceType(object):
    LOWEST = 1
    ASSIGN = 2
    EQUALS = 3
    CONDITIONAL = 4
    LESSGREATER = 5
    SUM = 6
    PRODUCT = 7
    PREFIX = 8
    CALL = 9
    INDEX = 10
    MEMBER_ACCESS = 11

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
    TokenType.DOT: PrecedenceType.MEMBER_ACCESS
}
