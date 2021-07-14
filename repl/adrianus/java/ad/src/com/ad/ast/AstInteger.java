package com.ad.ast;

import com.ad.token.Token;

public class AstInteger extends AstNode {
	private Token token;
	private int value;
	
	public AstInteger() {
		type = AstNodeTypeEnum.INTEGER_LITERAL;
	}
	
	public AstInteger(Token token) {
		type = AstNodeTypeEnum.INTEGER_LITERAL;
		this.token = token;
	}

	public AstInteger(Token token, int value) {
		type = AstNodeTypeEnum.INTEGER_LITERAL;
		this.token = token;
		
	}

	@Override
	public String tokenLiteral() {
		return null;
	}

	@Override
	public String toString() {
		return null;
	}

	public Token getToken() {
		return token;
	}

	public void setToken(Token token) {
		this.token = token;
	}

	public int getValue() {
		return value;
	}

	public void setValue(int value) {
		this.value = value;
	}
}
