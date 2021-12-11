package com.ad.parser;

import com.ad.ast.AstNode;
import com.ad.ast.AstProgram;
import com.ad.lexer.Lexer;
import com.ad.token.Token;
import com.ad.token.TokenTypeEnum;

import java.util.HashMap;
import java.util.function.Function;
import java.util.function.Supplier;

public class ParserFunctional {
    private Lexer lexer;
    private String source;
    private Token currentToken;
    private Token peekToken;
    private HashMap<TokenTypeEnum, Supplier<AstNode>> prefixParseFns = new HashMap<>();
    private HashMap<TokenTypeEnum, Function<AstNode, AstNode>> infixParserFns = new HashMap<>();

    public ParserFunctional() {
        lexer = new Lexer();
        prefixParseFns.put(TokenTypeEnum.IDENT, this::parseIdentifier);
        prefixParseFns.put(TokenTypeEnum.INT, this::parseIntegerLiteral);
        prefixParseFns.put(TokenTypeEnum.BANG, this::parsePrefixExpression);
        prefixParseFns.put(TokenTypeEnum.MINUS, this::parsePrefixExpression);
        infixParserFns.put(TokenTypeEnum.PLUS, this::parseInfixExpression);
    }

    public void load(String s) {
        source = s;
        lexer.load(source);
        nextToken();
        nextToken();
    }

    public void buildProgramStatements(AstProgram program) {
        // TODO: implement this
        while (currentToken.getType() != TokenTypeEnum.EOF) {
            AstNode node = parseStatement();
            if (node != null) {
                program.statements.add(node);
            }
            nextToken();
        }
    }

    private void nextToken() {
        currentToken = peekToken;
        peekToken = lexer.nextToken();
    }

    private boolean currentTokenIs(TokenTypeEnum tte) {
        return currentToken.getType() == tte;
    }

    private boolean peekTokenIs(TokenTypeEnum tte) {
        return peekToken.getType() == tte;
    }

    private boolean expectToken(TokenTypeEnum tte) {
        if (peekTokenIs(tte)) {
            nextToken();
            return true;
        }
        return false;
    }

    private PrecedenceTypeEnum currentPrecedence() {
        if (TokenPrecedenceConverter.precedenceMap.containsKey(currentToken.getType())) {
            return TokenPrecedenceConverter.getPrecedence(currentToken.getType());
        }
        return PrecedenceTypeEnum.LOWEST;
    }

    private PrecedenceTypeEnum peekPrecedence() {
        if (TokenPrecedenceConverter.precedenceMap.containsKey(peekToken.getType())) {
            return TokenPrecedenceConverter.getPrecedence(peekToken.getType());
        }
        return PrecedenceTypeEnum.LOWEST;
    }

    private AstNode parseStatement() {
        if (currentToken.getType() == TokenTypeEnum.LET) {
            return parseLetStatement();
        } else if (currentToken.getType() == TokenTypeEnum.RETURN) {
            return parseReturnStatement();
        } else {
            return parseExpressionStatement();
        }
    }

    private AstNode parseLetStatement() {
        // TODO: implement this
        return null;
    }

    private AstNode parseReturnStatement() {
        // TODO: implement this
        return null;
    }

    private AstNode parseExpressionStatement() {
        // TODO: implement this
        return null;
    }

    private AstNode parseIdentifier() {
        // TODO: implement this
        return null;
    }

    private AstNode parseIntegerLiteral() {
        // TODO: implement this
        return null;
    }

    private AstNode parsePrefixExpression() {
        // TODO: implement this
        return null;
    }

    private AstNode parseInfixExpression(AstNode node) {
        // TODO: implement this
        return null;
    }

    private AstNode parseExpression(PrecedenceTypeEnum pte) {
        if (!prefixParseFns.containsKey(currentToken.getType())) {
            return null;
        }
        Supplier<AstNode> prefix = prefixParseFns.get(currentToken.getType());
        AstNode leftExpression = prefix.get();
        while (!peekTokenIs(TokenTypeEnum.SEMICOLON) && pte.ordinal() < peekPrecedence().ordinal()) {
            if (!infixParserFns.containsKey(peekToken.getType())) return leftExpression;
            Function<AstNode, AstNode> infix = infixParserFns.get(peekToken.getType());
            nextToken();
            leftExpression = infix.apply(leftExpression);
        }
        return leftExpression;
    }
}
