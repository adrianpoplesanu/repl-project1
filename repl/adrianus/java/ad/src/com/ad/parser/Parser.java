package com.ad.parser;

import com.ad.ast.*;
import com.ad.lexer.Lexer;
import com.ad.token.Token;
import com.ad.token.TokenTypeEnum;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.function.Function;
import java.util.function.Supplier;

public class Parser {
    private Lexer lexer;
    private String source;
    private Token currentToken;
    private Token peekToken;
    private HashMap<TokenTypeEnum, Supplier<AstNode>> prefixParseFns = new HashMap<>();
    private HashMap<TokenTypeEnum, Function<AstNode, AstNode>> infixParseFns = new HashMap<>();

    public Parser() {
        lexer = new Lexer();
        prefixParseFns.put(TokenTypeEnum.IDENT, this::parseIdentifier);
        prefixParseFns.put(TokenTypeEnum.INT, this::parseIntegerLiteral);
        prefixParseFns.put(TokenTypeEnum.BANG, this::parsePrefixExpression);
        prefixParseFns.put(TokenTypeEnum.MINUS, this::parsePrefixExpression);
        prefixParseFns.put(TokenTypeEnum.TRUE, this::parseBoolean);
        prefixParseFns.put(TokenTypeEnum.FALSE, this::parseBoolean);
        prefixParseFns.put(TokenTypeEnum.LPAREN, this::parseGroupedExpression);
        prefixParseFns.put(TokenTypeEnum.IF, this::parseIfExpression);
        prefixParseFns.put(TokenTypeEnum.FUNCTION, this::parseFunctionLiteral);
        prefixParseFns.put(TokenTypeEnum.WHILE, this::parseWhileExpression);
        prefixParseFns.put(TokenTypeEnum.FOR, this::parseForExpression);
        prefixParseFns.put(TokenTypeEnum.DEF, this::parseDefStatement);
        prefixParseFns.put(TokenTypeEnum.LBRACKET, this::parseListLiteral);
        prefixParseFns.put(TokenTypeEnum.LBRACE, this::parseHashLiteral);
        prefixParseFns.put(TokenTypeEnum.STRING, this::parseStringLiteral);
        prefixParseFns.put(TokenTypeEnum.CLASS, this::parseClassStatement);
        prefixParseFns.put(TokenTypeEnum.PLUSPLUS, this::parsePrefixPlusPlus);
        prefixParseFns.put(TokenTypeEnum.NULL, this::parseNullExpression);
        prefixParseFns.put(TokenTypeEnum.THIS, this::parseThisExpression);
        infixParseFns.put(TokenTypeEnum.PLUS, this::parseInfixExpression);
        infixParseFns.put(TokenTypeEnum.MINUS, this::parseInfixExpression);
        infixParseFns.put(TokenTypeEnum.ASTERISK, this::parseInfixExpression);
        infixParseFns.put(TokenTypeEnum.SLASH, this::parseInfixExpression);
        infixParseFns.put(TokenTypeEnum.EQ, this::parseInfixExpression);
        infixParseFns.put(TokenTypeEnum.NOT_EQ, this::parseInfixExpression);
        infixParseFns.put(TokenTypeEnum.LT, this::parseInfixExpression);
        infixParseFns.put(TokenTypeEnum.GT, this::parseInfixExpression);
        infixParseFns.put(TokenTypeEnum.LTE, this::parseInfixExpression);
        infixParseFns.put(TokenTypeEnum.GTE, this::parseInfixExpression);
        infixParseFns.put(TokenTypeEnum.LPAREN, this::parseCallExpression);
        infixParseFns.put(TokenTypeEnum.LBRACKET, this::parseIndexExpression);
        infixParseFns.put(TokenTypeEnum.ASSIGN, this::parseAssignExpression);
        infixParseFns.put(TokenTypeEnum.DOT, this::parseMemberAccess);
        infixParseFns.put(TokenTypeEnum.PLUSPLUS, this::parsePostfixPlusPlus);
    }

    public void load(String s) {
        source = s;
        lexer.load(source);
        nextToken();
        nextToken();
    }

    public void buildProgramStatements(AstProgram program) {
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

    private boolean expectPeek(TokenTypeEnum tte) {
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
        AstLetStatement stmt = new AstLetStatement(currentToken);
        if (!expectPeek(TokenTypeEnum.IDENT)) {
            return null;
        }
        stmt.setName(new AstIdentifier(currentToken, currentToken.getLiteral()));
        if (!expectPeek(TokenTypeEnum.ASSIGN)) {
            return null;
        }
        nextToken();
        stmt.setValue(parseExpression(PrecedenceTypeEnum.LOWEST));
        if (currentTokenIs(TokenTypeEnum.SEMICOLON)) {
            nextToken();
        }
        return stmt;
    }

    private AstNode parseReturnStatement() {
        AstReturnStatement stmt = new AstReturnStatement(currentToken);
        nextToken();
        stmt.setValue(parseExpression(PrecedenceTypeEnum.LOWEST));
        if (peekTokenIs(TokenTypeEnum.SEMICOLON) || peekTokenIs(TokenTypeEnum.RBRACE) || peekTokenIs(TokenTypeEnum.EOF)) {
            return stmt;
        }
        while(!currentTokenIs(TokenTypeEnum.SEMICOLON) && !currentTokenIs(TokenTypeEnum.RBRACE) && !currentTokenIs(TokenTypeEnum.EOF)) {
            nextToken();
        }
        return stmt;
    }

    private AstNode parseExpressionStatement() {
        AstExpressionStatement stmt = new AstExpressionStatement(currentToken);
        stmt.setExpression(parseExpression(PrecedenceTypeEnum.LOWEST));
        if (peekTokenIs(TokenTypeEnum.SEMICOLON)) {
            nextToken();
        }
        return stmt;
    }

    private AstNode parseIdentifier() {
        return new AstIdentifier(getCurrentToken(), getCurrentToken().getLiteral());
    }

    private AstNode parseBoolean() {
        return new AstBoolean(getCurrentToken(), currentTokenIs(TokenTypeEnum.TRUE));
    }

    private AstNode parseIntegerLiteral() {
        String value = getCurrentToken().getLiteral();
        return new AstInteger(getCurrentToken(), Integer.parseInt(value));
    }

    private AstNode parseIfExpression() {
        AstIfExpression expr = new AstIfExpression(getCurrentToken());
        if (!expectPeek(TokenTypeEnum.LPAREN)) {
            return null;
        }
        nextToken();
        expr.setCondition(parseExpression(PrecedenceTypeEnum.LOWEST));
        if (!expectPeek(TokenTypeEnum.RPAREN)) {
            return null;
        }
        if (!expectPeek(TokenTypeEnum.LBRACE)) {
            return null;
        }
        expr.setConsequence(parseBlockStatement());
        if (peekTokenIs(TokenTypeEnum.ELSE)) {
            nextToken();
            if (!expectPeek(TokenTypeEnum.LBRACE)) {
                return null;
            }
            expr.setAlternative(parseBlockStatement());
        }
        return expr;
    }

    private AstNode parseBlockStatement() {
        AstBlockStatement block = new AstBlockStatement(getCurrentToken());
        nextToken();
        while(!currentTokenIs(TokenTypeEnum.RBRACE) && !currentTokenIs(TokenTypeEnum.EOF)) {
            AstNode statement = parseStatement();
            if (statement != null) {
                block.addStatement(statement);
            }
            nextToken();
        }
        return block;
    }

    private AstNode parseGroupedExpression() {
        nextToken();
        AstNode expr = parseExpression(PrecedenceTypeEnum.LOWEST);
        if (!expectPeek(TokenTypeEnum.RPAREN)) {
            return null;
        }
        return expr;
    }

    private AstNode parseFunctionLiteral() {
        AstFunctionLiteral func = new AstFunctionLiteral(getCurrentToken());
        if (!expectPeek(TokenTypeEnum.LPAREN)) {
            return null;
        }
        func.setParameters(parseFunctionParameters());
        if (!expectPeek(TokenTypeEnum.LBRACE)) {
            return null;
        }
        func.setBody(parseBlockStatement());
        return func;
    }

    private ArrayList<AstNode> parseFunctionParameters() {
        ArrayList<AstNode> identifiers = new ArrayList<AstNode>();
        if (peekTokenIs(TokenTypeEnum.RPAREN)) {
            nextToken();
            return identifiers;
        }
        nextToken();
        AstIdentifier ident = new AstIdentifier(getCurrentToken(), getCurrentToken().getLiteral());
        identifiers.add(ident);
        while(peekTokenIs(TokenTypeEnum.COMMA)) {
            nextToken();
            nextToken();
            ident = new AstIdentifier(getCurrentToken(), getCurrentToken().getLiteral());
            identifiers.add(ident);
        }
        if (!expectPeek(TokenTypeEnum.RPAREN)) {
            return new ArrayList<AstNode>(); // return an empty list
        }
        return identifiers;
    }

    private AstNode parseWhileExpression() {
        AstWhileExpression expr = new AstWhileExpression(getCurrentToken());
        if (!expectPeek(TokenTypeEnum.LPAREN)) {
            return null; // this should all return error ast nodes that evaluate in error objects
        }
        nextToken();
        expr.setCondition(parseExpression(PrecedenceTypeEnum.LOWEST));
        if (!expectPeek(TokenTypeEnum.RPAREN)) {
            return null; // see previous comment
        }
        if (!expectPeek(TokenTypeEnum.LBRACE)) {
            return null; // see previous comment
        }
        expr.setBody(parseBlockStatement());
        return expr;
    }

    private AstNode parseForExpression() {
        // TODO: implement this
        return null;
    }

    private AstNode parseDefStatement() {
        AstDefStatement stmt = new AstDefStatement(currentToken);
        nextToken();
        AstNode name = new AstIdentifier(currentToken, currentToken.getLiteral());
        stmt.setName(name);
        if (!expectPeek(TokenTypeEnum.LPAREN)) {
            // TODO: return an Error AST node
            return null;
        }
        ArrayList<AstNode> parameters = parseFunctionParameters();
        stmt.setParameters(parameters);
        if (!expectPeek(TokenTypeEnum.LBRACE)) {
            // TODO: return an Error AST node
            return null;
        }
        AstNode body = parseBlockStatement();
        stmt.setBody(body);
        return stmt;
    }

    private AstNode parseListLiteral() {
        AstListExpression expr = new AstListExpression(currentToken);
        expr.setElements(parseListExpressions());
        return expr;
    }

    private List<AstNode> parseListExpressions() {
        ArrayList<AstNode> elements = new ArrayList<>();
        if (peekTokenIs(TokenTypeEnum.RBRACKET)) {
            nextToken();
            return elements;
        }
        nextToken();
        AstNode element = parseExpression(PrecedenceTypeEnum.LOWEST);
        elements.add(element);
        while (peekTokenIs(TokenTypeEnum.COMMA)) {
            nextToken();
            nextToken();
            element = parseExpression(PrecedenceTypeEnum.LOWEST);
            elements.add(element);
        }
        if (!expectPeek(TokenTypeEnum.RBRACKET)) {
            return new ArrayList<>();
        }
        return elements;
    }

    private AstNode parseHashLiteral() {
        AstHashExpression hash = new AstHashExpression(currentToken);
        while(!peekTokenIs(TokenTypeEnum.RBRACE)) {
            nextToken();
            AstNode key = parseExpression(PrecedenceTypeEnum.LOWEST);
            if (!expectPeek(TokenTypeEnum.COLON)) {
                return null;
            }
            nextToken();
            AstNode value = parseExpression(PrecedenceTypeEnum.LOWEST);
            hash.AddPair(key, value);
            if (!peekTokenIs(TokenTypeEnum.RBRACE) && !(expectPeek(TokenTypeEnum.COMMA))) {
                return null;
            }
        }
        if (!expectPeek(TokenTypeEnum.RBRACE)) {
            return null;
        }
        return hash;
    }

    private AstNode parseStringLiteral() {
        AstStringLiteral node = new AstStringLiteral(getCurrentToken());
        node.setValue(getCurrentToken().getLiteral());
        return node;
    }

    private AstNode parsePrefixExpression() {
        AstPrefixExpression expr = new AstPrefixExpression(getCurrentToken(),
                getCurrentToken().getLiteral());
        nextToken();
        expr.setRight(parseExpression(PrecedenceTypeEnum.PREFIX));
        return expr;
    }

    private AstNode parseInfixExpression(AstNode left) {
        AstInfixExpression expr = new AstInfixExpression(getCurrentToken(), getCurrentToken().getLiteral(), left);
        PrecedenceTypeEnum precedence = currentPrecedence();
        nextToken();
        expr.setRight(parseExpression(precedence));
        return expr;
    }

    private AstNode parseCallExpression(AstNode node) {
        AstCallExpression expr = new AstCallExpression(getCurrentToken(), node);
        expr.setArguments(parseCallArguments());
        return expr;
    }

    private ArrayList<AstNode> parseCallArguments() {
        ArrayList<AstNode> args = new ArrayList<AstNode>();
        if (peekTokenIs(TokenTypeEnum.RPAREN)) {
            nextToken();
            return args;
        }
        nextToken();
        args.add(parseExpression(PrecedenceTypeEnum.LOWEST));
        while(peekTokenIs(TokenTypeEnum.COMMA)) {
            nextToken();
            nextToken();
            args.add(parseExpression(PrecedenceTypeEnum.LOWEST));
        }
        if (!expectPeek(TokenTypeEnum.RPAREN)) {
            return new ArrayList<AstNode>();
        }
        return args;
    }

    private AstNode parseIndexExpression(AstNode left) {
        AstIndexExpression expr = new AstIndexExpression(currentToken, left);
        nextToken();
        AstNode index = parseExpression(PrecedenceTypeEnum.LOWEST);
        expr.setIndex(index);
        if (!expectPeek(TokenTypeEnum.RBRACKET)) {
            return null;
        }
        return expr;
    }

    private AstNode parseAssignExpression(AstNode left) {
        AstAssignStatement stmt = new AstAssignStatement(currentToken);
        stmt.setName(left);
        nextToken();
        AstNode right = parseExpression(PrecedenceTypeEnum.LOWEST);
        stmt.setValue(right);
        if (currentTokenIs(TokenTypeEnum.SEMICOLON)) {
            nextToken();
        }
        return stmt;
    }

    private AstNode parseClassStatement() {
        AstClassStatement expr = new AstClassStatement(currentToken);
        nextToken();
        AstNode name = parseIdentifier();
        expr.setName(name);
        nextToken();
        while(!currentTokenIs(TokenTypeEnum.RBRACE)) {
            if (currentTokenIs(TokenTypeEnum.DEF)) {
                AstNode stmt = parseDefStatement();
                expr.getMethods().add(stmt);
            }
            if (currentTokenIs(TokenTypeEnum.IDENT)) {
                AstNode stmt = parseExpressionStatement();
                expr.getAttributes().add(stmt);
            }
            nextToken();
        }
        return expr;
    }

    private AstNode parsePrefixPlusPlus() {
        // TODO: implement this
        return null;
    }

    private AstNode parseNullExpression() {
        AstNullExpression expr = new AstNullExpression(currentToken);
        return expr;
    }

    private AstNode parseThisExpression() {
        // TODO: implement this
        return null;
    }

    private AstNode parseMemberAccess(AstNode left) {
        // TODO: implement this
        return null;
    }

    private AstNode parsePostfixPlusPlus(AstNode left) {
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
            if (!infixParseFns.containsKey(peekToken.getType())) return leftExpression;
            Function<AstNode, AstNode> infix = infixParseFns.get(peekToken.getType());
            nextToken();
            leftExpression = infix.apply(leftExpression);
        }
        return leftExpression;
    }

    public Lexer getLexer() {
        return lexer;
    }

    public void setLexer(Lexer lexer) {
        this.lexer = lexer;
    }

    public String getSource() {
        return source;
    }

    public void setSource(String source) {
        this.source = source;
    }

    public Token getCurrentToken() {
        return currentToken;
    }

    public void setCurrentToken(Token currentToken) {
        this.currentToken = currentToken;
    }

    public Token getPeekToken() {
        return peekToken;
    }

    public void setPeekToken(Token peekToken) {
        this.peekToken = peekToken;
    }

    public HashMap<TokenTypeEnum, Supplier<AstNode>> getPrefixParseFns() {
        return prefixParseFns;
    }

    public void setPrefixParseFns(HashMap<TokenTypeEnum, Supplier<AstNode>> prefixParseFns) {
        this.prefixParseFns = prefixParseFns;
    }

    public HashMap<TokenTypeEnum, Function<AstNode, AstNode>> getInfixParseFns() {
        return infixParseFns;
    }

    public void setInfixParseFns(HashMap<TokenTypeEnum, Function<AstNode, AstNode>> infixParseFns) {
        this.infixParseFns = infixParseFns;
    }
}
