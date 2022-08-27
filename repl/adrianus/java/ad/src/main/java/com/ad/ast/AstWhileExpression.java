package com.ad.ast;

import com.ad.token.Token;

public class AstWhileExpression extends AstNode {
	private Token token;
	private AstNode condition;
	private AstNode body;
	
	public AstWhileExpression(Token token) {
		type = AstNodeTypeEnum.WHILE_EXPRESSION;
		this.token = token;
	}

	@Override
	public String tokenLiteral() {
		return token.getLiteral();
	}

	@Override
	public String toString() {
		return "WhileExpression [Token: " + token + "]";
	}

	public Token getToken() {
		return token;
	}

	public void setToken(Token token) {
		this.token = token;
	}

	public AstNode getCondition() {
		return condition;
	}

	public void setCondition(AstNode condition) {
		this.condition = condition;
	}

	public AstNode getBody() {
		return body;
	}

	public void setBody(AstNode body) {
		this.body = body;
	}
}
