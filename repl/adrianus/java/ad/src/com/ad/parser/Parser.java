package com.ad.parser;

import java.util.HashMap;

import com.ad.ast.AstExpressionStatement;
import com.ad.ast.AstIdentifier;
import com.ad.ast.AstLetStatement;
import com.ad.ast.AstNode;
import com.ad.ast.AstProgram;
import com.ad.ast.AstReturnStatement;
import com.ad.lexer.Lexer;
import com.ad.token.Token;
import com.ad.token.TokenTypeEnum;

public class Parser {
	private Lexer lexer;
	private String source;
	private Token currentToken;
	private Token peekToken;
	private HashMap<TokenTypeEnum, PrefixParseInterface> prefixParseFns = new HashMap<TokenTypeEnum, PrefixParseInterface>();
	private HashMap<TokenTypeEnum, InfixParseInterface> infixParseFns = new HashMap<TokenTypeEnum, InfixParseInterface>();
	
	public Parser() {
		//... i should instantiate the callback maps here for prefix and infix
		lexer = new Lexer();
		prefixParseFns.put(TokenTypeEnum.IDENT, new IdentifierParser(this));
		prefixParseFns.put(TokenTypeEnum.INT, new IntegerLiteralParser(this));
		prefixParseFns.put(TokenTypeEnum.BANG, new PrefixExpressionParser(this));
		prefixParseFns.put(TokenTypeEnum.MINUS, new PrefixExpressionParser(this));
		prefixParseFns.put(TokenTypeEnum.TRUE, new BooleanParser());
		prefixParseFns.put(TokenTypeEnum.FALSE, new BooleanParser());
		prefixParseFns.put(TokenTypeEnum.LPAREN, new GroupExpressionParser());
	    prefixParseFns.put(TokenTypeEnum.IF, new IfExpressionParser());
	    prefixParseFns.put(TokenTypeEnum.FUNCTION, new FunctionLiteralParser());
	    prefixParseFns.put(TokenTypeEnum.WHILE, new WhileExpressionParser());
		infixParseFns.put(TokenTypeEnum.PLUS, new InfixExpressionParser());
		infixParseFns.put(TokenTypeEnum.MINUS, new InfixExpressionParser());
		infixParseFns.put(TokenTypeEnum.ASTERISK, new InfixExpressionParser());
		infixParseFns.put(TokenTypeEnum.SLASH, new InfixExpressionParser());
		infixParseFns.put(TokenTypeEnum.LT, new InfixExpressionParser());
		infixParseFns.put(TokenTypeEnum.GT, new InfixExpressionParser());
		infixParseFns.put(TokenTypeEnum.LTE, new InfixExpressionParser());
		infixParseFns.put(TokenTypeEnum.GTE, new InfixExpressionParser());
		infixParseFns.put(TokenTypeEnum.LPAREN, new CallExpressionParser());
	}

	public void load(String source) {
		this.source = source;
		lexer.load(this.source);
		nextToken();
		nextToken();
	}
	
	public void buildProgramStatements(AstProgram program) {
		while (currentToken.getType() != TokenTypeEnum.EOF) {
			//System.out.println(currentToken);
			//nextToken();
			AstNode node = parseStatement();
			System.out.println(node);
			nextToken();
		}
	}
	
	public void nextToken() {
		currentToken = peekToken;
		peekToken = lexer.nextToken();
	}
	
	public boolean currentTokenIs(TokenTypeEnum tte) {
		return currentToken.getType() == tte;
	}

	public boolean peekTokenIs(TokenTypeEnum tte) {
		return peekToken.getType() == tte;
	}

	public boolean ExpectPeek(TokenTypeEnum tte) {
		if (peekTokenIs(tte)) {
			nextToken();
			return true;
		} else {
			return false;
		}
	}
	
	private PrecedenceTypeEnum currentPrecedence() {
		if (TokenPrecedenceConverter.precedenceMap.containsKey(currentToken.getType())) {
			return TokenPrecedenceConverter.precedenceMap.get(currentToken.getType());
		}
		return PrecedenceTypeEnum.LOWEST;
	}

	private PrecedenceTypeEnum peekPrecedence() {
		if (TokenPrecedenceConverter.precedenceMap.containsKey(peekToken.getType())) {
			return TokenPrecedenceConverter.precedenceMap.get(peekToken.getType());
		}
		return PrecedenceTypeEnum.LOWEST;
	}

	public AstNode parseStatement() {
		if (currentToken.getType() == TokenTypeEnum.LET) {
			return parseLetStatement();
		} else if (currentToken.getType() == TokenTypeEnum.RETURN) {
			return parseReturnStatement();
		} else {
			return parseExpressionStatement();
		}
	}

	public AstNode parseLetStatement() {
		AstLetStatement stmt = new AstLetStatement(currentToken);
		if (!ExpectPeek(TokenTypeEnum.IDENT)) {
			return null;
		}
		stmt.setName(new AstIdentifier(currentToken, currentToken.getLiteral()));
		if (!ExpectPeek(TokenTypeEnum.ASSIGN)) {
			return null;
		}
		nextToken();
		stmt.setValue(parseExpression(PrecedenceTypeEnum.LOWEST));
		return stmt;
	}

	public AstNode parseReturnStatement() {
		AstReturnStatement stmt = new AstReturnStatement(currentToken);
		nextToken();
		stmt.setValue(parseExpression(PrecedenceTypeEnum.LOWEST));
		while(!currentTokenIs(TokenTypeEnum.SEMICOLON) && !currentTokenIs(TokenTypeEnum.EOF)) {
			nextToken();
		}
		return stmt;
	}

	public AstNode parseExpressionStatement() {
		AstExpressionStatement stmt = new AstExpressionStatement(currentToken);
		stmt.setExpression(parseExpression(PrecedenceTypeEnum.LOWEST));
		if (peekTokenIs(TokenTypeEnum.SEMICOLON)) {
			nextToken();
		}
		return stmt;
	}

	public AstNode parsePrefixExpression() {
		return null;
	}

	public AstNode parseInfixExpression() {
		return null;
	}

	public AstNode parseExpression(PrecedenceTypeEnum pte) {
		if (!prefixParseFns.containsKey(currentToken.getType())) {
			System.out.println("no prefixParseFns mapping found for " + pte);
			return null;
		}
		PrefixParseInterface prefixParser = prefixParseFns.get(currentToken.getType());
		AstNode left_expression = prefixParser.parse();
		while(!peekTokenIs(TokenTypeEnum.SEMICOLON) && (pte.ordinal() < peekPrecedence().ordinal())) {
			if (!infixParseFns.containsKey(peekToken.getType())) return left_expression;
			InfixParseInterface infixParser = infixParseFns.get(peekToken.getType());
			nextToken();
			System.out.println(left_expression);
		}
		return left_expression;
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

	public HashMap<TokenTypeEnum, PrefixParseInterface> getPrefixParseFns() {
		return prefixParseFns;
	}

	public void setPrefixParseFns(HashMap<TokenTypeEnum, PrefixParseInterface> prefixParseFns) {
		this.prefixParseFns = prefixParseFns;
	}

	public HashMap<TokenTypeEnum, InfixParseInterface> getInfixParseFns() {
		return infixParseFns;
	}

	public void setInfixParseFns(HashMap<TokenTypeEnum, InfixParseInterface> infixParseFns) {
		this.infixParseFns = infixParseFns;
	}
}
