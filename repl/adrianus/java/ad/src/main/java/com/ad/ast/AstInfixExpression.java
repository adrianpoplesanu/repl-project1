package com.ad.ast;

import com.ad.token.Token;

public class AstInfixExpression extends AstNode {
	private Token token;
	private String operator;
	private AstNode left;
	private AstNode right;
	
	public AstInfixExpression() {
		type = AstNodeTypeEnum.INFIX_EXPRESSION;
	}
	
	public AstInfixExpression(Token token) {
		type = AstNodeTypeEnum.INFIX_EXPRESSION; 
		this.token = token;
	}
	
	public AstInfixExpression(Token token, String operator) {
		type = AstNodeTypeEnum.INFIX_EXPRESSION;
		this.token = token;
		this.operator = operator;
	}
	
	public AstInfixExpression(Token token, String operator, AstNode left) {
		type = AstNodeTypeEnum.INFIX_EXPRESSION;
		this.token = token;
		this.operator = operator;
		this.left = left;
	}

	@Override
	public String tokenLiteral() {
		return token.getLiteral();
	}

	@Override
	public String toString() {
		return "InfixExpression [operator: " + operator + " right: " + right + " left: " + left + "]";
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

	public AstNode getLeft() {
		return left;
	}

	public void setLeft(AstNode left) {
		this.left = left;
	}

	public AstNode getRight() {
		return right;
	}

	public void setRight(AstNode right) {
		this.right = right;
	}
}
