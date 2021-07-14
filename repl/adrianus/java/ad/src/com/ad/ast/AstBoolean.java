package com.ad.ast;

import com.ad.token.Token;

public class AstBoolean extends AstNode {
	private Token token;
	private boolean value;
	
	public AstBoolean() {
		type = AstNodeTypeEnum.BOOLEAN;
	}
	
	public AstBoolean(Token token) {
		type = AstNodeTypeEnum.BOOLEAN;
		this.token = token;
	}

	public AstBoolean(Token token, boolean value) {
		type = AstNodeTypeEnum.BOOLEAN;
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

	public boolean isValue() {
		return value;
	}

	public void setValue(boolean value) {
		this.value = value;
	}
}
