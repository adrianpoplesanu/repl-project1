package com.ad.ast;

import com.ad.token.Token;

public class AstIfExpression extends AstNode {
    private Token token;
    private AstNode condition;
    private AstNode consequence;
    private AstNode alternative;
    
    public AstIfExpression(Token token) {
    	type = AstNodeTypeEnum.IF_EXPRESSION;
    	this.token = token;
    }

	@Override
	public String tokenLiteral() {
		return token.getLiteral();
	}

	@Override
	public String toString() {
		return "IfExpression [token: " + token + "]";
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

	public AstNode getConsequence() {
		return consequence;
	}

	public void setConsequence(AstNode consequence) {
		this.consequence = consequence;
	}

	public AstNode getAlternative() {
		return alternative;
	}

	public void setAlternative(AstNode alternative) {
		this.alternative = alternative;
	}
}
