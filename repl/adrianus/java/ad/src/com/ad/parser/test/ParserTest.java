package com.ad.parser.test;

import com.ad.ast.AstNodeUtils;
import com.ad.ast.AstProgram;
import com.ad.parser.Parser;
import com.ad.parser.ParserOLD;

public class ParserTest {
	public static final boolean RUN_TESTS = false;
	
    public static void testParser() {
    	//String source = "let a = 1; let add = fn(a,b) { 1+2+3; } ; werty; ; 1 + 2 - 3 * 4; add(a, 747); if (3 > 1) { 1001; } else { 1002; } let a = true; let b = false;";
    	String source = "let a = 1;";
    	
    	if (RUN_TESTS) {
			Parser parser = new Parser();
			AstProgram program = new AstProgram();
			parser.load(source);
			parser.buildProgramStatements(program);
			AstNodeUtils.PrintASTNode(program, 0);
    	}
    }
}
