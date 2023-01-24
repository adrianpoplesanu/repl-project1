package com.ad.builtin;

import com.ad.ast.AstAssignStatement;
import com.ad.ast.AstDefStatement;
import com.ad.ast.AstExpressionStatement;
import com.ad.environment.Environment;
import com.ad.objects.*;

import java.util.ArrayList;
import java.util.List;

public class GetattrsBuiltin extends AbstractBuiltin {
    public GetattrsBuiltin() {
        super();
        acceptedParameterSize.add(1);
    }

    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        if (args.size() == 1) {
            AdObject target = args.get(0);
            List<AdObject> elements = new ArrayList<>();
            if (target.getType() == ObjectTypeEnum.INSTANCE) {
                AdClassInstance klassInstance = (AdClassInstance) target;
                klassInstance.getEnvironment().populateGetattrs().stream().forEach(attrName -> elements.add(new AdStringObject(attrName)));
                AdListObject result = new AdListObject(elements);
                return result;
            }
            if (target.getType() == ObjectTypeEnum.CLASS) {
                AdClassObject klassObject = (AdClassObject) target;
                klassObject.getAttributes().stream().forEach(attrNode -> {
                    AstExpressionStatement stmt = (AstExpressionStatement) attrNode;
                    AstAssignStatement assignStatement = (AstAssignStatement) stmt.getExpression();
                    elements.add(new AdStringObject(assignStatement.getName().tokenLiteral()));
                });
                klassObject.getMethods().stream().forEach(attrNode -> {
                    String methodName = ((AstDefStatement) attrNode).getName().tokenLiteral();
                    elements.add(new AdStringObject(methodName));
                });
                AdListObject result = new AdListObject(elements);
                return result;
            }
            return new AdErrorObject("getattrs can only be called on a class instance");
        }
        return null;
    }
}
