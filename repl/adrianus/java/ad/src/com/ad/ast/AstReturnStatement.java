package com.ad.ast;

import com.ad.token.Token;

public class AstReturnStatement extends AstNode {
	private Token token;
	private AstNode value;

	public AstReturnStatement(Token token) {
		type = AstNodeTypeEnum.RETURN_STATEMENT;
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

	public AstNode getValue() {
		return value;
	}

	public void setValue(AstNode value) {
		this.value = value;
	}
}
