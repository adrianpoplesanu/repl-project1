package com.ad.ast;

import com.ad.token.Token;

import java.util.ArrayList;

public class AstProgram extends AstNode {
	public ArrayList<AstNode> statements;
	
	public AstProgram() {
		type = AstNodeTypeEnum.PROGRAM;
		statements = new ArrayList<AstNode>();
	}

	public void reset() {
		statements.clear();
	}

	@Override
	public String tokenLiteral() {
		// ProgramStatement has no token
		return null;
	}

	@Override
	public Token getToken() {
		return null;
	}

	@Override
	public String toString() {
		return "Program";
	}
}
