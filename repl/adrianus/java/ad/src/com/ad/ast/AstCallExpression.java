package com.ad.ast;

import java.util.ArrayList;

import com.ad.token.Token;

public class AstCallExpression extends AstNode {
	private Token token;
	private ArrayList<AstNode> arguments;
	private AstNode function;
	
	public AstCallExpression(Token token) {
		this.token = token;
	}
	
	public AstCallExpression(Token token, AstNode function) {
		this.token = token;
		this.function = function;
	}

	@Override
	public String tokenLiteral() {
		return token.getLiteral();
	}

	@Override
	public String toString() {
		String out = "";
		return out;
	}

	public Token getToken() {
		return token;
	}

	public void setToken(Token token) {
		this.token = token;
	}

	public ArrayList<AstNode> getArguments() {
		return arguments;
	}

	public void setArguments(ArrayList<AstNode> arguments) {
		this.arguments = arguments;
	}

	public AstNode getFunction() {
		return function;
	}

	public void setFunction(AstNode function) {
		this.function = function;
	}
}
