package com.ad.ast;

import com.ad.token.Token;

public class AstLetStatement extends AstNode {
	private Token token;
	
	public AstLetStatement(Token token) {
		this.token = token;
	}

	@Override
	public String tokenLiteral() {
		return token.getLiteral();
	}

	@Override
	public String toString() {
		return "AstLetStatement [token: ..., value: ...]";
	}

}
