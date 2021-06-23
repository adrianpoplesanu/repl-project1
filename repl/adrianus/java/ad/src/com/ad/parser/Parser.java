package com.ad.parser;

import com.ad.ast.AstProgram;
import com.ad.lexer.Lexer;
import com.ad.token.Token;
import com.ad.token.TokenTypeEnum;

public class Parser {
	private Lexer lexer;
	private String source;
	private Token currentToken;
	private Token peekToken;
	
	public Parser() {
		//... i should instantiate the callback maps here for prefix and infix
		lexer = new Lexer();
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
}
