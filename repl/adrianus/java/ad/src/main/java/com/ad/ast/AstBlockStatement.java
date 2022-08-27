package com.ad.ast;

import java.util.ArrayList;

import com.ad.token.Token;

public class AstBlockStatement extends AstNode {
	private Token token;
	private ArrayList<AstNode> statements;
	
	public AstBlockStatement() {
		type = AstNodeTypeEnum.BLOCK_STATEMENT;
		statements = new ArrayList<AstNode>();
	}
	
	public AstBlockStatement(Token token) {
		this();
		this.token = token;
	}
	
	public void addStatement(AstNode statement) {
		statements.add(statement);
	}

	@Override
	public String tokenLiteral() {
		return token.getLiteral();
	}

	@Override
	public String toString() {
		return "BlockStatement [Token: " + token + "]";
	}

	public Token getToken() {
		return token;
	}

	public void setToken(Token token) {
		this.token = token;
	}

	public ArrayList<AstNode> getStatements() {
		return statements;
	}

	public void setStatements(ArrayList<AstNode> statements) {
		this.statements = statements;
	}
}
