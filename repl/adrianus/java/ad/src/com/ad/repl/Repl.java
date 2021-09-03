package com.ad.repl;

import java.util.Scanner;

import com.ad.ast.AstProgram;
import com.ad.environment.Environment;
import com.ad.evaluator.Evaluator;
import com.ad.parser.Parser;

public class Repl {
	private Scanner scanner = new Scanner(System.in);
	private Parser parser;
	private AstProgram program;
	private Evaluator evaluator;
	
	public Repl(Parser parser, AstProgram program, Evaluator evaluator) {
		this.parser = parser;
		this.program = program;
		this.evaluator = evaluator;
	}
	
    public String getNextLine() {
    	return scanner.nextLine();
    }
    
    public void loop() {
    	Environment env = Environment.newEnvironment();
    	while(true) {
    		String source = getNextLine();
    		if (source.equals("exit()")) break;
    		parser.load(source);
    		parser.buildProgramStatements(program);
    		this.evaluator.eval(program, env);
    	}
    }
}
