from lexer import Lexer
from token_type import TokenType
from precedence_type import PrecedenceType, precedences
from ast import ASTLetStatement, ASTIdentifier, ASTReturnStatement, ASTExpressionStatement, \
                ASTBoolean, ASTInteger, ASTPrefixExpression, ASTIfExpression, \
                ASTCallExpression, ASTInfixExpression, ASTFunctionLiteral, \
                ASTBlockStatement, ASTStringLiteral, ASTListLiteral, ASTIndexExpression, \
                ASTHashLiteral, ASTWhileExpression, ASTAssignStatement, ASTDefStatement, \
                ASTClassStatement, ASTMemberAccess, ASTComment, ASTPrefixIncrement, \
                ASTPostfixIncrement, ASTForExpression, ASTNullExpression, ASTThisExpression, \
                ASTFloat, ASTSuperExpression, ASTBrakeStatement, ASTContinueStatement, ASTPlusEqualsStatement


class Parser(object):
    def __init__(self):
        self.lexer = Lexer()
        self.source = ''
        self.current_token = None
        self.peek_token = None
        self.prefix_parse_functions = {}
        self.infix_parse_functions = {}
        self.statement_parse_functions = {}
        self.errors = []
        self.statement_parse_functions[TokenType.IF] = self.parse_if_statement
        self.statement_parse_functions[TokenType.DEF] = self.parse_def_statement

        self.prefix_parse_functions[TokenType.IDENT] = self.parse_identifier
        self.prefix_parse_functions[TokenType.INT] = self.parse_integer_literal
        self.prefix_parse_functions[TokenType.FLOAT] = self.parse_float_literal
        self.prefix_parse_functions[TokenType.BANG] = self.parse_prefix_expression
        self.prefix_parse_functions[TokenType.MINUS] = self.parse_prefix_expression
        self.prefix_parse_functions[TokenType.TRUE] = self.parse_boolean
        self.prefix_parse_functions[TokenType.FALSE] = self.parse_boolean
        self.prefix_parse_functions[TokenType.LPAREN] = self.parse_grouped_expression
        self.prefix_parse_functions[TokenType.FUN] = self.parse_fun_expression
        self.prefix_parse_functions[TokenType.FUNCTION] = self.parse_function_expression
        self.prefix_parse_functions[TokenType.CLASS] = self.parse_class_statement
        self.prefix_parse_functions[TokenType.WHILE] = self.parse_while_expression
        self.prefix_parse_functions[TokenType.FOR] = self.parse_for_expression
        self.prefix_parse_functions[TokenType.FUNC] = self.parse_func_literal
        self.prefix_parse_functions[TokenType.STRING] = self.parse_string_literal
        self.prefix_parse_functions[TokenType.LBRACKET] = self.parse_list_literal
        self.prefix_parse_functions[TokenType.LBRACE] = self.parse_hash_literal
        self.prefix_parse_functions[TokenType.PLUSPLUS] = self.parse_prefix_plus_plus
        self.prefix_parse_functions[TokenType.MINUSMINUS] = self.parse_prefix_minus_minus
        self.prefix_parse_functions[TokenType.NULL] = self.parse_null_expression
        self.prefix_parse_functions[TokenType.THIS] = self.parse_this_expression
        self.prefix_parse_functions[TokenType.SUPER] = self.parse_super_expression

        self.infix_parse_functions[TokenType.PLUS] = self.parse_infix_expression
        self.infix_parse_functions[TokenType.MINUS] = self.parse_infix_expression
        self.infix_parse_functions[TokenType.SLASH] = self.parse_infix_expression
        self.infix_parse_functions[TokenType.ASTERISK] = self.parse_infix_expression
        self.infix_parse_functions[TokenType.MODULUS] = self.parse_infix_expression
        self.infix_parse_functions[TokenType.EQ] = self.parse_infix_expression
        self.infix_parse_functions[TokenType.NOT_EQ] = self.parse_infix_expression
        self.infix_parse_functions[TokenType.LT] = self.parse_infix_expression
        self.infix_parse_functions[TokenType.GT] = self.parse_infix_expression
        self.infix_parse_functions[TokenType.LTE] = self.parse_infix_expression
        self.infix_parse_functions[TokenType.GTE] = self.parse_infix_expression
        self.infix_parse_functions[TokenType.AND] = self.parse_infix_expression
        self.infix_parse_functions[TokenType.OR] = self.parse_infix_expression
        self.infix_parse_functions[TokenType.LPAREN] = self.parse_call_expression
        self.infix_parse_functions[TokenType.ASSIGN] = self.parse_assign_expression
        self.infix_parse_functions[TokenType.LBRACKET] = self.parse_index_expression
        self.infix_parse_functions[TokenType.DOT] = self.parse_member_access
        self.infix_parse_functions[TokenType.PLUSPLUS] = self.parse_infix_plus_plus
        self.infix_parse_functions[TokenType.MINUSMINUS] = self.parse_infix_minus_minus
        self.infix_parse_functions[TokenType.PLUS_EQ] = self.parse_plus_equals_expression
        self.infix_parse_functions[TokenType.MINUS_EQ] = self.parse_plus_equals_expression

    def reset(self, source):
        self.source = source
        self.lexer.load(source)
        self.next_token()
        self.next_token()

    def build_program_statements(self, program):
        while self.current_token.type != TokenType.EOF:
            #print str(self.current_token)
            stmt = self.parse_statement()
            if stmt:
                program.statements.append(stmt)
            self.next_token()

    def next_token(self):
        self.current_token = self.peek_token
        self.peek_token = self.lexer.next_token()

    def current_token_is(self, token_type):
        return self.current_token.type == token_type

    def peek_token_is(self, token_type):
        return self.peek_token.type == token_type

    def expect_peek(self, token_type):
        if self.peek_token_is(token_type):
            self.next_token()
            return True
        else:
            self.errors.append('ERROR: i was expecting a different token here: ' + token_type)
            return False

    def peek_precedence(self):
        if self.peek_token.type in precedences:
            return precedences[self.peek_token.type]
        return PrecedenceType.LOWEST

    def current_precedence(self):
        if self.current_token.type in precedences:
            return precedences[self.current_token.type]
        return PrecedenceType.LOWEST

    def parse_statement(self):
        if self.current_token.type in self.statement_parse_functions:
            return self.statement_parse_functions[self.current_token.type]()
        if self.current_token.type == TokenType.LET:
            return self.parse_let_statement()
        elif self.current_token.type == TokenType.RETURN:
            return self.parse_return_statement()
        elif self.current_token.type == TokenType.BREAK:
            return self.parse_break_statement()
        elif self.current_token.type == TokenType.CONTINUE:
            return self.parse_continue_statement()
        elif self.current_token.type == TokenType.START_COMMENT:
            return self.parse_comment()
        elif self.current_token.type == TokenType.MULTI_COMMENT:
            return self.parse_multi_comment()
        elif self.current_token.type == TokenType.SINGLE_COMMENT:
            return self.parse_single_comment()
        return self.parse_expression_statement()

    def parse_let_statement(self):
        stmt = ASTLetStatement(token=self.current_token)
        if not self.expect_peek(TokenType.IDENT):
            return None
        stmt.name = ASTIdentifier(token=self.current_token, value=self.current_token.literal)
        if not self.expect_peek(TokenType.ASSIGN):
            return None
        self.next_token()
        stmt.value = self.parse_expression(PrecedenceType.LOWEST)
        if self.current_token_is(TokenType.SEMICOLON):
            self.next_token()
        return stmt

    def parse_return_statement(self):
        stmt = ASTReturnStatement(token=self.current_token)
        self.next_token()
        stmt.value = self.parse_expression(PrecedenceType.LOWEST)
        if self.peek_token_is(TokenType.SEMICOLON) or self.peek_token_is(TokenType.RBRACE) or self.peek_token_is(TokenType.EOF):
            return stmt
        while not self.current_token_is(TokenType.SEMICOLON) and not self.current_token_is(TokenType.RBRACE) and not self.current_token_is(TokenType.EOF):
            self.next_token()
        return stmt

    def parse_break_statement(self):
        return ASTBrakeStatement(self.current_token)

    def parse_continue_statement(self):
        return ASTContinueStatement(self.current_token)

    def parse_expression_statement(self):
        stmt = ASTExpressionStatement(token=self.current_token)
        stmt.expression = self.parse_expression(PrecedenceType.LOWEST)
        if self.peek_token_is(TokenType.SEMICOLON):
            self.next_token()
        return stmt

    def parse_identifier(self):
        stmt = ASTIdentifier(token=self.current_token, value=self.current_token.literal)
        return stmt

    def parse_integer_literal(self):
        stmt = ASTInteger(token=self.current_token, value=int(self.current_token.literal))
        return stmt

    def parse_float_literal(self):
        stmt = ASTFloat(token=self.current_token, value=float(self.current_token.literal))
        return stmt

    def parse_prefix_expression(self):
        expr = ASTPrefixExpression(token=self.current_token, operator=self.current_token.literal)
        self.next_token()
        expr.right = self.parse_expression(PrecedenceType.PREFIX)
        return expr

    def parse_boolean(self):
        stmt = ASTBoolean(token=self.current_token, value=self.current_token_is(TokenType.TRUE))
        return stmt

    def parse_grouped_expression(self):
        self.next_token()
        expr = self.parse_expression(PrecedenceType.LOWEST)
        if not self.expect_peek(TokenType.RPAREN):
            return None
        return expr

    def parse_if_statement(self):
        expr = ASTIfExpression(token=self.current_token)
        if not self.expect_peek(TokenType.LPAREN):
            return None
        self.next_token()
        expr.condition = self.parse_expression(PrecedenceType.LOWEST)
        if not self.expect_peek(TokenType.RPAREN):
            return None
        if not self.expect_peek(TokenType.LBRACE):
            expr.consequence = self.parse_single_block_statement()
        else:
            expr.consequence = self.parse_block_statement()
        if self.peek_token_is(TokenType.ELSE):
            self.next_token()
            if not self.expect_peek(TokenType.LBRACE):
                expr.alternative = self.parse_single_block_statement()
            else:
                expr.alternative = self.parse_block_statement()
        return expr

    def parse_block_statement(self):
        block = ASTBlockStatement(token=self.current_token)
        self.next_token()
        while (not self.current_token_is(TokenType.RBRACE)) and (not self.current_token_is(TokenType.EOF)):
            stmt = self.parse_statement()
            if stmt:
                block.statements.append(stmt)
            self.next_token()
        return block

    def parse_single_block_statement(self):
        block = ASTBlockStatement(token=self.current_token)
        self.next_token()
        stmt = self.parse_statement()
        block.statements.append(stmt)
        return block

    def parse_def_statement(self):
        stmt = ASTDefStatement(token=self.current_token)
        self.next_token()
        name = ASTIdentifier(token=self.current_token, value=self.current_token.literal)
        stmt.name = name
        if not self.expect_peek(TokenType.LPAREN):
            # this should return an error object
            return None
        stmt.parameters = self.parse_function_parameters()
        if not self.expect_peek(TokenType.LBRACE):
            return None
        stmt.body = self.parse_block_statement()
        return stmt

    def parse_fun_expression(self):
        # this is exactly like def
        stmt = ASTDefStatement(token=self.current_token)
        self.next_token()
        name = ASTIdentifier(token=self.current_token, value=self.current_token.literal)
        stmt.name = name
        if not self.expect_peek(TokenType.LPAREN):
            # this should return an error object
            return None
        stmt.parameters = self.parse_function_parameters()
        if not self.expect_peek(TokenType.LBRACE):
            return None
        stmt.body = self.parse_block_statement()
        return stmt

    def parse_function_expression(self):
        # this is exactly like def
        stmt = ASTDefStatement(token=self.current_token)
        self.next_token()
        name = ASTIdentifier(token=self.current_token, value=self.current_token.literal)
        stmt.name = name
        if not self.expect_peek(TokenType.LPAREN):
            # this should return an error object
            return None
        stmt.parameters = self.parse_function_parameters()
        if not self.expect_peek(TokenType.LBRACE):
            return None
        stmt.body = self.parse_block_statement()
        return stmt

    def parse_func_literal(self):
        func = ASTFunctionLiteral(token=self.current_token)
        if not self.expect_peek(TokenType.LPAREN):
            return None
        func.parameters = self.parse_function_parameters()
        if not self.expect_peek(TokenType.LBRACE):
            return None
        func.body = self.parse_block_statement()
        return func

    def parse_function_parameters(self):
        identifiers = []
        if self.peek_token_is(TokenType.RPAREN):
            self.next_token()
            return identifiers
        self.next_token()
        ident = ASTIdentifier(token=self.current_token, value=self.current_token.literal)
        identifiers.append(ident)
        while self.peek_token_is(TokenType.COMMA):
            self.next_token()
            self.next_token()
            ident = ASTIdentifier(token=self.current_token, value=self.current_token.literal)
            identifiers.append(ident)
        if not self.expect_peek(TokenType.RPAREN):
            return []
        return identifiers

    def parse_infix_expression(self, left):
        expr = ASTInfixExpression(token=self.current_token, operator=self.current_token.literal, left=left)
        precedence = self.current_precedence()
        self.next_token()
        expr.right = self.parse_expression(precedence)
        return expr

    def parse_call_expression(self, func):
        # TODO: this call expression might be a class constructor
        expr = ASTCallExpression(self.current_token, func)
        expr.arguments = self.parse_call_arguments()
        return expr

    def parse_call_arguments(self):
        args = []
        if self.peek_token_is(TokenType.RPAREN):
            self.next_token()
            return args
        self.next_token()
        args.append(self.parse_expression(PrecedenceType.LOWEST))
        while self.peek_token_is(TokenType.COMMA):
            self.next_token()
            self.next_token()
            args.append(self.parse_expression(PrecedenceType.LOWEST))
        if not self.expect_peek(TokenType.RPAREN):
            return []
        return args

    def parse_while_expression(self):
        expr = ASTWhileExpression(token=self.current_token)
        if not self.expect_peek(TokenType.LPAREN):
            return None
        self.next_token()
        expr.condition = self.parse_expression(PrecedenceType.LOWEST)
        if not self.expect_peek(TokenType.RPAREN):
            return None
        if not self.expect_peek(TokenType.LBRACE):
            expr.block = self.parse_single_block_statement()
        else:
            expr.block = self.parse_block_statement()
        return expr

    def parse_string_literal(self):
        return ASTStringLiteral(token=self.current_token, value=str(self.current_token.literal))

    def parse_list_literal(self):
        expr = ASTListLiteral(self.current_token)
        expr.elements = self.parse_expression_list()
        return expr

    def parse_expression_list(self):
        elements = []
        if self.peek_token_is(TokenType.RBRACKET):
            self.next_token()
            return elements
        self.next_token()
        elements.append(self.parse_expression(PrecedenceType.LOWEST))
        while self.peek_token_is(TokenType.COMMA):
            self.next_token()
            self.next_token()
            elements.append(self.parse_expression(PrecedenceType.LOWEST))
        if not self.expect_peek(TokenType.RBRACKET):
            return None
        return elements

    def parse_index_expression(self, left):
        expr = ASTIndexExpression(token=self.current_token, left=left)
        self.next_token()
        expr.index = self.parse_expression(PrecedenceType.LOWEST)
        if not self.expect_peek(TokenType.RBRACKET):
            return None
        return expr

    def parse_hash_literal(self):
        hash = ASTHashLiteral(token=self.current_token)
        hash.pairs = {}
        while not self.peek_token_is(TokenType.RBRACE):
            self.next_token()
            key = self.parse_expression(PrecedenceType.LOWEST)
            if not self.expect_peek(TokenType.COLON):
                return None
            self.next_token()
            value = self.parse_expression(PrecedenceType.LOWEST)
            hash.pairs[key] = value
            if not self.peek_token_is(TokenType.RBRACE) and not self.expect_peek(TokenType.COMMA):
                return None
        if not self.expect_peek(TokenType.RBRACE):
            return None
        return hash

    def parse_assign_expression(self, left):
        stmt = ASTAssignStatement(token=self.current_token, value=self.current_token.literal)
        stmt.name = left
        self.next_token()
        stmt.value = self.parse_expression(PrecedenceType.LOWEST)
        if self.current_token_is(TokenType.SEMICOLON):
            self.next_token()
        return stmt

    def parse_comment(self):
        comment = ASTComment(token=self.current_token)
        while not self.current_token_is(TokenType.EOF) and not self.current_token_is(TokenType.END_COMMENT):
            self.next_token()
        return comment

    def parse_multi_comment(self):
        comment = ASTComment(token=self.current_token)
        return comment

    def parse_single_comment(self):
        comment = ASTComment(token=self.current_token)
        return comment

    def parse_class_statement(self):
        expr = ASTClassStatement(token=self.current_token)
        self.next_token()
        name = self.parse_identifier()
        expr.name = name
        self.next_token()
        expr.attributes = []
        expr.methods = []
        expr.inherit_from = []

        if self.current_token_is(TokenType.COLON):
            self.next_token()
            while not self.current_token_is(TokenType.LBRACE):
                parent = self.parse_identifier()
                expr.inherit_from.append(parent)
                self.next_token()

        while not self.current_token_is(TokenType.RBRACE):
            if self.current_token_is(TokenType.DEF):
                stmt = self.parse_def_statement()
                expr.methods.append(stmt)
            if self.current_token_is(TokenType.FUN):
                stmt = self.parse_def_statement()
                expr.methods.append(stmt)
            elif self.current_token_is(TokenType.IDENT):
                stmt = self.parse_expression_statement()
                expr.attributes.append(stmt)
            self.next_token()
        return expr

    def parse_member_access(self, left):
        member_access = ASTMemberAccess(token=self.current_token)
        self.next_token()
        right = ASTIdentifier(token=self.current_token, value=self.current_token.literal)
        member_access.owner = left
        member_access.member = right

        if self.peek_token_is(TokenType.LPAREN):
            self.next_token()
            member_access.arguments = self.parse_call_arguments()
            member_access.is_method = True
        else:
            member_access.arguments = []
            member_access.is_method = False
        return member_access

    def parse_prefix_plus_plus(self):
        stmt = ASTPrefixIncrement(token=self.current_token)
        stmt.operator = self.current_token.literal
        self.next_token()
        name = ASTIdentifier(token=self.current_token, value=self.current_token.literal)
        stmt.name = name
        return stmt

    def parse_prefix_minus_minus(self):
        stmt = ASTPrefixIncrement(token=self.current_token)
        stmt.operator = self.current_token.literal
        self.next_token()
        name = ASTIdentifier(token=self.current_token, value=self.current_token.literal)
        stmt.name = name
        return stmt

    def parse_infix_plus_plus(self, left):
        stmt = ASTPostfixIncrement(token=self.current_token)
        stmt.name = left
        stmt.operator = self.current_token.literal
        return stmt

    def parse_infix_minus_minus(self, left):
        stmt = ASTPostfixIncrement(token=self.current_token)
        stmt.name = left
        stmt.operator = self.current_token.literal
        return stmt

    def parse_plus_equals_expression(self, left):
        stmt = ASTPlusEqualsStatement(token=self.current_token)
        stmt.name = left
        self.next_token()
        stmt.value = self.parse_expression(PrecedenceType.LOWEST)
        if self.current_token_is(TokenType.SEMICOLON):
            self.next_token()
        return stmt

    def parse_for_expression(self):
        stmt = ASTForExpression(token=self.current_token)
        self.next_token()
        if not self.current_token_is(TokenType.LPAREN):
            print ('error parsing for expression: LPRAEN expected')
            return None
        self.next_token()
        stmt.initialization = self.parse_expression(PrecedenceType.LOWEST)
        if not self.expect_peek(TokenType.SEMICOLON):
            print ('error parsing for expression: SEMICOLON expected')
            return None
        self.next_token()
        stmt.condition = self.parse_expression(PrecedenceType.LOWEST)
        if not self.expect_peek(TokenType.SEMICOLON):
            print ('error parsing for expression: second SEMICOLON expected')
            return None
        self.next_token()
        stmt.step = self.parse_expression(PrecedenceType.LOWEST)
        if not self.expect_peek(TokenType.RPAREN):
            print ('error parsing for expression: RPRAREN not found')
            return None
        if not self.expect_peek(TokenType.LBRACE):
            stmt.body =  self.parse_single_block_statement()
        else:
            stmt.body =  self.parse_block_statement()
        return stmt

    def parse_null_expression(self):
        expr = ASTNullExpression(self.current_token)
        return expr

    def parse_this_expression(self):
        expr = ASTThisExpression(self.current_token)
        return expr

    def parse_super_expression(self):
        expr = ASTSuperExpression(self.current_token)
        self.next_token()

        if not self.current_token_is(TokenType.LPAREN):
            return None
        self.next_token()

        target = self.parse_identifier()
        expr.target = target

        if not self.expect_peek(TokenType.RPAREN):
            return None

        return expr

    def parse_expression(self, precedence):
        if self.current_token.type not in self.prefix_parse_functions:
            return None
        prefix = self.prefix_parse_functions[self.current_token.type]
        left_exp = prefix()
        while (not self.peek_token_is(TokenType.SEMICOLON)) and precedence < self.peek_precedence():
            if self.peek_token.type not in self.infix_parse_functions:
                return left_exp
            infix = self.infix_parse_functions[self.peek_token.type]
            self.next_token()
            left_exp = infix(left_exp)
        return left_exp
