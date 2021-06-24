package com.ad.parser;

import java.util.HashMap;

import com.ad.ast.AstLetStatement;
import com.ad.ast.AstNode;
import com.ad.ast.AstProgram;
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
		prefixParseFns.put(TokenTypeEnum.IDENT, new IdentifierParser());
		infixParseFns.put(TokenTypeEnum.PLUS, new InfixExpressionParser());
	}
	
	public void load(String source) {
		this.source = source;
		lexer.load(this.source);
		nextToken();
		nextToken();
	}
	
	public void buildProgramStatements(AstProgram program) {
		while (currentToken.getType() != TokenTypeEnum.EOF) {
			System.out.println(currentToken);
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
		// TODO: implement the let statement parsing logic
		AstLetStatement stmt = new AstLetStatement(currentToken);
		return stmt;
	}
	
	public AstNode parseReturnStatement() {
		return null;
	}
	
	public AstNode parseExpressionStatement() {
		return null;
	}
}
