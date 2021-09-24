package com.ad.ast;

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
	public String toString() {
		return "Program";
	}
}
