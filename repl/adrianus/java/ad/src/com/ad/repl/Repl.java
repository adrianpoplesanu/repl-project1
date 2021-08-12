package com.ad.repl;

import java.util.Scanner;

import com.ad.ast.AstProgram;
import com.ad.parser.Parser;

public class Repl {
	private Scanner scanner = new Scanner(System.in);
	private Parser parser;
	private AstProgram program;
	
	public Repl(Parser parser, AstProgram program) {
		this.parser = parser;
		this.program = program;
	}
	
    public String getNextLine() {
    	return scanner.nextLine();
    }
    
    public void loop() {
    	while(true) {
    		String source = getNextLine();
    		if (source.equals("exit()")) break;
    		parser.load(source);
    		parser.buildProgramStatements(program);
    	}
    }
}
