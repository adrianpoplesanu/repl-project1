package com.ad.ast;

import java.util.ArrayList;

public class AstProgram extends AstNode {
	public ArrayList<AstNode> statements;
	
	public AstProgram() {
		type = AstNodeTypeEnum.PROGRAM;
		statements = new ArrayList<AstNode>();
	}

	public void reset() {
		
	}

	@Override
	public String tokenLiteral() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public String toString() {
		// TODO Auto-generated method stub
		return null;
	}
}
