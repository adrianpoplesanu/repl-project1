package com.ad.builtin;

import com.ad.ast.AstProgram;
import com.ad.environment.Environment;
import com.ad.evaluator.Evaluator;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;
import com.ad.parser.Parser;

import java.util.List;

public class EvalBuiltin extends AbstractBuiltin {

    public EvalBuiltin() {
        super();
        acceptedParameterSize.add(1);
    }

    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        String unescapedSource = ((AdStringObject) args.get(0)).getValue();
        String source = unescapedSource.replace("\\\"", "\"").replace("\\\'","\'");
        Parser parser = new Parser();
        parser.load(source);
        AstProgram program = new AstProgram();
        parser.buildProgramStatements(program);
        Evaluator evaluator = new Evaluator();
        evaluator.eval(program, env);
        return null;
    }
}
