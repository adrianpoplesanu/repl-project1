package com.ad.ast;

import com.ad.token.Token;

public class AstIdentifier extends AstNode {
	private Token token;
	private String value;

	public AstIdentifier(Token token, String value) {
		this.token = token;
		this.value = value;
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

	public String getValue() {
		return value;
	}

	public void setValue(String value) {
		this.value = value;
	}
}
