package com.ad.ast;

import com.ad.token.Token;

public class AstStringLiteral extends AstNode {
	private Token token;
	private String value;
	
	public AstStringLiteral(Token token) {
		type = AstNodeTypeEnum.STRING_LITERAL;
		this.token = token;
	}

	@Override
	public String tokenLiteral() {
		return token.getLiteral();
	}

	@Override
	public String toString() {
		// TODO Auto-generated method stub
		return null;
	}

	public Token getToken() {
		return token;
	}

	public void setToken(Token token) {
		this.token = token;
	}

	public String getValue() {
		return value;
	}

	public void setValue(String value) {
		this.value = value;
	}
}
