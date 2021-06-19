package com.ad.ast;

import java.util.ArrayList;

public class AstProgram {
	ArrayList<AstNode> statements;
	
	public AstProgram() {
		statements = new ArrayList<AstNode>();
	}
}
