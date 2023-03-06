package com.ad.builtin;

import com.ad.ast.AstProgram;
import com.ad.environment.Environment;
import com.ad.evaluator.Evaluator;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;
import com.ad.parser.Parser;
import com.ad.utils.EvalUtils;
import com.ad.utils.FileUtils;

import java.util.List;

public class ImportBuiltin extends AbstractBuiltin {

    public ImportBuiltin() {
        super();
        acceptedParameterSize.add(1);
    }

    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        String filepath = ((AdStringObject) args.get(0)).getValue();
        String source = FileUtils.readFile(filepath);
        EvalUtils.evalSource(source, env);
        return null;
    }
}
