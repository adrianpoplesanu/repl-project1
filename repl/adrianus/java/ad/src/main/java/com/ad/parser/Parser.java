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

    private HashMap<TokenTypeEnum, Supplier<AstNode>> statementParseFns = new HashMap<>();

    public Parser() {
        lexer = new Lexer();
        statementParseFns.put(TokenTypeEnum.IF, this::parseIfExpression);
        statementParseFns.put(TokenTypeEnum.DEF, this::parseDefStatement);

        prefixParseFns.put(TokenTypeEnum.IDENT, this::parseIdentifier); // equivalent to: () -> parseIdentifier()
        prefixParseFns.put(TokenTypeEnum.INT, this::parseIntegerLiteral);
        prefixParseFns.put(TokenTypeEnum.FLOAT, this::parseFloatLiteral);
        prefixParseFns.put(TokenTypeEnum.BANG, this::parsePrefixExpression);
        prefixParseFns.put(TokenTypeEnum.MINUS, this::parsePrefixExpression);
        prefixParseFns.put(TokenTypeEnum.TRUE, this::parseBoolean);
        prefixParseFns.put(TokenTypeEnum.FALSE, this::parseBoolean);
        prefixParseFns.put(TokenTypeEnum.LPAREN, this::parseGroupedExpression);
        prefixParseFns.put(TokenTypeEnum.FUNCTION, this::parseFunctionStatement);
        prefixParseFns.put(TokenTypeEnum.WHILE, this::parseWhileExpression);
        prefixParseFns.put(TokenTypeEnum.FOR, this::parseForExpression);
        prefixParseFns.put(TokenTypeEnum.FUN, this::parseFunStatement);
        prefixParseFns.put(TokenTypeEnum.FUNC, this::parseFunctionLiteral);
        prefixParseFns.put(TokenTypeEnum.METHOD, this::parseFunctionLiteral);
        prefixParseFns.put(TokenTypeEnum.LBRACKET, this::parseListLiteral);
        prefixParseFns.put(TokenTypeEnum.LBRACE, this::parseHashLiteral);
        prefixParseFns.put(TokenTypeEnum.DOUBLE_QUOTES, this::parseStringLiteral);
        prefixParseFns.put(TokenTypeEnum.SINGLE_QUOTES, this::parseStringLiteral);
        prefixParseFns.put(TokenTypeEnum.CLASS, this::parseClassStatement);
        prefixParseFns.put(TokenTypeEnum.PLUSPLUS, this::parsePrefixPlusPlus);
        prefixParseFns.put(TokenTypeEnum.MINUSMINUS, this::parsePrefixMinusMinus);
        prefixParseFns.put(TokenTypeEnum.NULL, this::parseNullExpression);
        prefixParseFns.put(TokenTypeEnum.THIS, this::parseThisExpression);
        prefixParseFns.put(TokenTypeEnum.SUPER, this::parseSuperExpression);

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
        infixParseFns.put(TokenTypeEnum.PLUS_EQ, this::parsePlusEqualsExpression);
        infixParseFns.put(TokenTypeEnum.MINUS_EQ, this::parsePlusEqualsExpression);
        infixParseFns.put(TokenTypeEnum.DOT, this::parseMemberAccess);
        infixParseFns.put(TokenTypeEnum.PLUSPLUS, this::parsePostfixExpression);
        infixParseFns.put(TokenTypeEnum.MINUSMINUS, this::parsePostfixExpression);
        infixParseFns.put(TokenTypeEnum.MODULUS, this::parseInfixExpression);
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
        if (statementParseFns.containsKey(currentToken.getType())) {
            return statementParseFns.get(currentToken.getType()).get();
        }

        if (currentToken.getType() == TokenTypeEnum.LET) {
            return parseLetStatement();
        } else if (currentToken.getType() == TokenTypeEnum.RETURN) {
            return parseReturnStatement();
        } else if (currentToken.getType() == TokenTypeEnum.BREAK) {
            return parseBreakStatement();
        } else if (currentToken.getType() == TokenTypeEnum.CONTINUE) {
            return parseContinueStatement();
        } else if (currentToken.getType() == TokenTypeEnum.START_COMMENT) {
            return parseCommentStatement();
        } else if (currentToken.getType() == TokenTypeEnum.MULTI_COMMENT) {
            return parseMultiCommentStatement();
        } else if (currentToken.getType() == TokenTypeEnum.SINGLE_COMMENT) {
            return parseSingleCommentStatement();
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
        //while(!currentTokenIs(TokenTypeEnum.SEMICOLON) && !currentTokenIs(TokenTypeEnum.RBRACE) && !currentTokenIs(TokenTypeEnum.EOF)) {
        //    nextToken();
        //}
        //nextToken();
        return stmt;
    }

    private AstNode parseCommentStatement() {
        AstComment node = new AstComment(currentToken);
        while(!currentTokenIs(TokenTypeEnum.END_COMMENT)) {
            nextToken();
        }
        return node;
    }

    private AstNode parseMultiCommentStatement() {
        AstComment node = new AstComment(currentToken);
        return node;
    }

    private AstNode parseSingleCommentStatement() {
        AstComment node = new AstComment(currentToken);
        return node;
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

    private AstNode parseFloatLiteral() {
        String value = getCurrentToken().getLiteral();
        return new AstFloat(getCurrentToken(), Float.parseFloat(value));
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
            expr.setConsequence(parseSingleBlockStatement());
        } else {
            expr.setConsequence(parseBlockStatement());
        }
        if (peekTokenIs(TokenTypeEnum.ELSE)) {
            nextToken();
            if (!expectPeek(TokenTypeEnum.LBRACE)) {
                expr.setAlternative(parseSingleBlockStatement());
            } else {
                expr.setAlternative(parseBlockStatement());
            }
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

    private AstNode parseSingleBlockStatement() {
        AstBlockStatement block = new AstBlockStatement(getCurrentToken());
        nextToken();
        AstNode statement = parseStatement();
        if (statement != null) {
            block.addStatement(statement);
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
            expr.setBody(parseSingleBlockStatement());
        } else {
            expr.setBody(parseBlockStatement());
        }
        return expr;
    }

    private AstNode parseBreakStatement() {
        AstBreakStatement stmt = new AstBreakStatement(currentToken);
        return stmt;
    }

    private AstNode parseContinueStatement() {
        AstContinueStatement stmt = new AstContinueStatement(currentToken);
        return stmt;
    }

    private AstNode parseForExpression() {
        AstForExpression expr = new AstForExpression(currentToken);
        nextToken();
        if (!currentTokenIs(TokenTypeEnum.LPAREN)) {
            return null;
        }
        nextToken();
        expr.setInitialization(parseExpression(PrecedenceTypeEnum.LOWEST));
        if (!expectPeek(TokenTypeEnum.SEMICOLON)) {
            return null;
        }
        nextToken();
        expr.setCondition(parseExpression(PrecedenceTypeEnum.LOWEST));
        if (!expectPeek(TokenTypeEnum.SEMICOLON)) {
            return null;
        }
        nextToken();
        expr.setStep(parseExpression(PrecedenceTypeEnum.LOWEST));
        if (!expectPeek(TokenTypeEnum.RPAREN)) {
            return null;
        }
        if (!expectPeek(TokenTypeEnum.LBRACE)) {
            expr.setBody(parseSingleBlockStatement());
        } else {
            expr.setBody(parseBlockStatement());
        }
        return expr;
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

    private AstNode parseFunStatement() {
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

    private AstNode parseFunctionStatement() {
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
        if (currentTokenIs(TokenTypeEnum.COLON)) {
            AstNode index = new AstNullExpression();
            expr.setIndex(index);
        } else {
            AstNode index = parseExpression(PrecedenceTypeEnum.LOWEST);
            expr.setIndex(index);
            nextToken();
            if (currentTokenIs(TokenTypeEnum.RBRACKET)) {
                return expr;
            }
        }

        nextToken();

        if (currentTokenIs(TokenTypeEnum.COLON)) {
            AstNode indexEnd = new AstNullExpression();
            expr.setIndexEnd(indexEnd);
        } else {
            AstNode indexEnd = parseExpression(PrecedenceTypeEnum.LOWEST);
            expr.setIndexEnd(indexEnd);
            nextToken();
            if (currentTokenIs(TokenTypeEnum.RBRACKET)) {
                AstNode step = new AstInteger(currentToken, 1);
                expr.setStep(step);
                return expr;
            }
        }

        nextToken();

        if (currentTokenIs(TokenTypeEnum.RBRACKET)) {
            AstNode step = new AstInteger(currentToken, 1);
            expr.setStep(step);
            return expr;
        } else {
            AstNode step = parseExpression(PrecedenceTypeEnum.LOWEST);
            nextToken();
            expr.setStep(step);
        }

        if (!currentTokenIs(TokenTypeEnum.RBRACKET)) {
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

    private AstNode parsePlusEqualsExpression(AstNode left) {
        AstPlusEqualsStatement stmt = new AstPlusEqualsStatement(currentToken);
        stmt.name = left;
        nextToken();
        stmt.value = parseExpression(PrecedenceTypeEnum.LOWEST);
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
        if (currentTokenIs(TokenTypeEnum.COLON)) {
            nextToken();
            while (!currentTokenIs(TokenTypeEnum.LBRACE)) {
                AstNode parent = parseIdentifier();
                expr.getInheritFrom().add(parent);
                nextToken();
            }
        }
        while(!currentTokenIs(TokenTypeEnum.RBRACE)) {
            if (currentTokenIs(TokenTypeEnum.DEF)) {
                AstNode stmt = parseDefStatement();
                expr.getMethods().add(stmt);
            }
            if (currentTokenIs(TokenTypeEnum.FUN)) {
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
        AstPrefixIncrement expr = new AstPrefixIncrement(currentToken);
        expr.setOperation(currentToken.getLiteral());
        nextToken();
        AstIdentifier identifier = new AstIdentifier(currentToken, currentToken.getLiteral());
        expr.setName(identifier);
        return expr;
    }

    private AstNode parsePrefixMinusMinus() {
        AstPrefixIncrement expr = new AstPrefixIncrement(currentToken);
        expr.setOperation(currentToken.getLiteral());
        nextToken();
        AstIdentifier identifier = new AstIdentifier(currentToken, currentToken.getLiteral());
        expr.setName(identifier);
        return expr;
    }

    private AstNode parseNullExpression() {
        AstNullExpression expr = new AstNullExpression(currentToken);
        return expr;
    }

    private AstNode parseThisExpression() {
        AstThisExpression expr = new AstThisExpression(currentToken);
        return expr;
    }

    private AstNode parseSuperExpression() {
        AstSuperExpression expr = new AstSuperExpression(currentToken);
        nextToken();
        if (!currentTokenIs(TokenTypeEnum.LPAREN)) {
            return null;
        }
        nextToken();
        AstNode target = parseIdentifier();
        expr.setTarget(target);
        if (!expectPeek(TokenTypeEnum.RPAREN)) {
            return null;
        }
        return expr;
    }

    private AstNode parseMemberAccess(AstNode left) {
        AstMemberAccess astMemberAccess = new AstMemberAccess(currentToken);
        nextToken();
        AstIdentifier right = new AstIdentifier(currentToken, currentToken.getLiteral());
        astMemberAccess.setOwner(left);
        astMemberAccess.setMember(right);
        if (peekTokenIs(TokenTypeEnum.LPAREN)) {
            nextToken();
            astMemberAccess.setIsMethod(true);
            astMemberAccess.setArguments(parseCallArguments());
        } else {
            astMemberAccess.setIsMethod(false);
            astMemberAccess.clear();
        }
        return astMemberAccess;
    }

    private AstNode parsePostfixExpression(AstNode left) {
        AstPostfixIncrement expr = new AstPostfixIncrement(currentToken);
        expr.setName(left);
        expr.setOperation(currentToken.getLiteral());
        return expr;
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
