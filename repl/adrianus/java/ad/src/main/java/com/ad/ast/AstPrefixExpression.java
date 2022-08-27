package com.ad.ast;

import com.ad.token.Token;

public class AstPrefixExpression extends AstNode {
	private Token token;
	private String operator;
	private AstNode right;
	
	public AstPrefixExpression() {
		type = AstNodeTypeEnum.PREFIX_EXPRESSION;
	}
	
	public AstPrefixExpression(Token token) {
		type = AstNodeTypeEnum.PREFIX_EXPRESSION;
		this.token = token;
	}
	
	public AstPrefixExpression(Token token, String operator) {
		type = AstNodeTypeEnum.PREFIX_EXPRESSION;
		this.token = token;
		this.operator = operator;
	}
	
	public AstPrefixExpression(Token token, String operator, AstNode right) {
		type = AstNodeTypeEnum.PREFIX_EXPRESSION;
		this.token = token;
		this.operator = operator;
		this.right = right;
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

	public String getOperator() {
		return operator;
	}

	public void setOperator(String operator) {
		this.operator = operator;
	}

	public AstNode getRight() {
		return right;
	}

	public void setRight(AstNode right) {
		this.right = right;
	}
}
