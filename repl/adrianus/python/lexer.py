from token import Token, TokenType
from token import keywords_map, token_type_map


class Lexer(object):
    def __init__(self, position=0, read_position=0, source=''):
        self.position = position
        self.read_position = read_position
        self.ch = ''
        self.source = source
        self.read_char()

    def load(self, source):
        self.position = 0
        self.read_position = 0
        self.ch = ''
        self.source = source
        self.read_char()

    def next_token(self):
        token = Token()
        self.skip_whitespaces()
        skip_read_char = False
        if self.ch == '+':
            token.type = TokenType.PLUS
            token.literal = self.ch
        elif self.ch == '-':
            token.type = TokenType.MINUS
            token.literal = self.ch
        elif self.ch == '/':
            token.type = TokenType.SLASH
            token.literal = self.ch
        elif self.ch == '*':
            token.type = TokenType.ASTERISK
            token.literal = self.ch
        elif self.ch == '>':
            if self.peek_char() == '=':
                self.read_char()
                token.type = TokenType.GTE
                token.literal = ">="
            else:
                token.type = TokenType.GT
                token.literal = self.ch
        elif self.ch == '<':
            if self.peek_char() == '=':
                token.type = TokenType.GTE
                token.literal = "<="
            else:
                token.type = TokenType.LT
                token.literal = self.ch
        elif self.ch == ';':
            token.type = TokenType.SEMICOLON
            token.literal = self.ch
        elif self.ch == '(':
            token.type = TokenType.LPAREN
            token.literal = self.ch
        elif self.ch == ')':
            token.type = TokenType.RPAREN
            token.literal = self.ch
        elif self.ch == '{':
            token.type = TokenType.LBRACE
            token.literal = self.ch
        elif self.ch == '}':
            token.type = TokenType.RBRACE
            token.literal = self.ch
        elif self.ch == ',':
            token.type = TokenType.COMMA
            token.literal = self.ch
        elif self.ch == 0:
            token.type = TokenType.EOF
            token.literal = self.ch
        elif self.ch == '"':
            token.type = TokenType.STRING
            token.literal = self.read_string()
        elif self.ch == '=':
            if self.peek_char() == '=':
                self.read_char()
                token.type = TokenType.EQ
                token.literal = '=='
            else:
                token.type = TokenType.ASSIGN
                token.literal = self.ch
        elif self.ch == '!':
            if self.peek_char() == '=':
                self.read_char()
                token.type = TokenType.NOT_EQ
                token.literal = '!='
            else:
                token.type = TokenType.BANG
                token.literal = '!'
        else:
            if self.is_letter():
                token.literal = self.read_ident()
                token.type = self.lookup_ident(token.literal)
                skip_read_char = True
            elif self.is_digit():
                token.literal = self.read_number()
                token.type = TokenType.INT
                skip_read_char = True
            else:
                token.type = TokenType.ILLEGAL
                token.literal = ''
        if not skip_read_char:
            self.read_char()
        return token

    def read_char(self):
        if self.read_position < len(self.source):
            self.ch = self.source[self.read_position]
        else:
            self.ch = 0
        self.position = self.read_position
        self.read_position += 1

    def skip_whitespaces(self):
        while self.ch == ' ' or self.ch == '\t' or self.ch == '\r' or self.ch == '\n':
            self.read_char()

    def is_letter(self):
        return ('a' <= self.ch and self.ch <= 'z') or ('A' <= self.ch and self.ch <= 'Z') or self.ch == '_'

    def is_digit(self):
        return '0' <= self.ch and self.ch <= '9'

    def lookup_ident(self, ident):
        if ident in keywords_map:
            return keywords_map[ident]
        return TokenType.IDENT

    def peek_char(self):
        if self.read_position >= len(self.source):
            return 0
        return self.source[self.read_position]

    def read_ident(self):
        start = self.position
        while self.is_letter():
            self.read_char()
        return self.source[start:self.position]

    def read_number(self):
        start = self.position
        while self.is_digit():
            self.read_char()
        return self.source[start:self.position]

    def read_string(self):
        self.read_char()
        start = self.position
        while self.ch != '"':
            self.read_char()
        return self.source[start:self.position]