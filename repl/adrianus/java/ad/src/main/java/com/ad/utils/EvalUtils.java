package com.ad.utils;

import com.ad.ast.AstProgram;
import com.ad.environment.Environment;
import com.ad.evaluator.Evaluator;
import com.ad.parser.Parser;

public class EvalUtils {
    public static void evalSource(String source, Environment env) {
        Parser parser = new Parser();
        parser.load(source);
        AstProgram program = new AstProgram();
        parser.buildProgramStatements(program);
        Evaluator evaluator = new Evaluator();
        evaluator.eval(program, env);
    }
}
