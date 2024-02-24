package com.ad.ast;

import java.util.ArrayList;

import com.ad.token.Token;

public class AstFunctionLiteral extends AstNode {
	private Token token;
	private ArrayList<AstNode> parameters;
	private AstNode body;
	
	public AstFunctionLiteral(Token token) {
		type = AstNodeTypeEnum.FUNCTION_LITERAL;
		this.token = token;
	}

	@Override
	public String tokenLiteral() {
		return token.getLiteral();
	}

	@Override
	public Token getToken() {
		return token;
	}

	@Override
	public String toString() {
		return "FunctionLiteral [" + token + "]";
	}

	public ArrayList<AstNode> getParameters() {
		return parameters;
	}

	public void setParameters(ArrayList<AstNode> parameters) {
		this.parameters = parameters;
	}

	public AstNode getBody() {
		return body;
	}

	public void setBody(AstNode body) {
		this.body = body;
	}
}
