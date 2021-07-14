package com.ad.ast;

import com.ad.token.Token;

public class AstLetStatement extends AstNode {
	private Token token;
	private AstNode value;
	private AstIdentifier name;
	
	public AstLetStatement(Token token) {
		type = AstNodeTypeEnum.LET_STATEMENT;
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

	public AstIdentifier getName() {
		return name;
	}

	public void setName(AstIdentifier name) {
		this.name = name;
	}
}
