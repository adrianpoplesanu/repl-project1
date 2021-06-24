package com.ad.ast;

import com.ad.token.Token;

public class AstLetStatement extends AstNode {
	private Token token;
	private AstNode value;
	private AstIdentifier node;
	
	public AstLetStatement(Token token) {
		this.token = token;
	}

	@Override
	public String tokenLiteral() {
		return token.getLiteral();
	}

	@Override
	public String toString() {
		return "LetStatement [token: " + token + ", value: " + value + "]";
	}

	public Token getToken() {
		return token;
	}

	public void setToken(Token token) {
		this.token = token;
	}

	public AstNode getValue() {
		return value;
	}

	public void setValue(AstNode value) {
		this.value = value;
	}

	public AstIdentifier getNode() {
		return node;
	}

	public void setNode(AstIdentifier node) {
		this.node = node;
	}
}
