package com.ad.repl;

import java.util.Scanner;

import com.ad.ast.AstProgram;
import com.ad.bootstrap.Bootstrap;
import com.ad.environment.Environment;
import com.ad.environment.EnvironmentUtils;
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
    	Environment bootstrapEnv = EnvironmentUtils.newEnvironment();
		Bootstrap bootstrap = new Bootstrap();
		bootstrap.loadBootstrap(program, parser, evaluator, bootstrapEnv);
		Environment env = EnvironmentUtils.newEnvironment();
		env.setBootstrap(bootstrapEnv);
    	while(true) {
    		System.out.print(">> ");
    		String source = getNextLine();
    		if (source.equals("exit()")) break;
    		parser.load(source);
    		program.reset();
    		parser.buildProgramStatements(program);
    		evaluator.eval(program, env);
    	}
    }

    public void executeSource(String source) {
		Environment bootstrapEnv = EnvironmentUtils.newEnvironment();
		Bootstrap bootstrap = new Bootstrap();
		bootstrap.loadBootstrap(program, parser, evaluator, bootstrapEnv);
		Environment env = EnvironmentUtils.newEnvironment();
		env.setBootstrap(bootstrapEnv);
		parser.load(source);
		program.reset();
		parser.buildProgramStatements(program);
		//AstNodeUtils.PrintASTNode(program, 0);
		evaluator.eval(program, env);
	}
}
