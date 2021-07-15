package com.ad.ast;

import com.ad.token.Token;

public class AstExpressionStatement extends AstNode {
    private Token token;
    private AstNode expression;
	
    public AstExpressionStatement() {
    	type = AstNodeTypeEnum.EXPRESSION_STATEMENT;
    }
    
    public AstExpressionStatement(Token token) {
    	type = AstNodeTypeEnum.EXPRESSION_STATEMENT;
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

	public AstNode getExpression() {
		return expression;
	}

	public void setExpression(AstNode expression) {
		this.expression = expression;
	}
}
