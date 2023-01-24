package com.ad.builtin;

import com.ad.ast.AstAssignStatement;
import com.ad.ast.AstExpressionStatement;
import com.ad.ast.AstNode;
import com.ad.environment.Environment;
import com.ad.objects.*;

import java.util.List;

public class GetattrBuiltin extends AbstractBuiltin {
    public GetattrBuiltin() {
        super();
        acceptedParameterSize.add(2);
    }

    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        AdObject target = args.get(0);
        String attrName = ((AdStringObject) args.get(1)).getValue();
        if (target.getType() == ObjectTypeEnum.INSTANCE) {
            AdClassInstance klassInstance = (AdClassInstance) target;
            return klassInstance.getEnvironment().lookupOnlyInStore(attrName);
        }
        if (target.getType() == ObjectTypeEnum.CLASS) {
            AdClassObject klassObject = (AdClassObject) target;
            for (AstNode node : klassObject.getAttributes()) {
                AstExpressionStatement stmt = (AstExpressionStatement) node;
                AstAssignStatement assignStatement = (AstAssignStatement) stmt.getExpression();
                if (assignStatement.getValue().tokenLiteral().equals(attrName)) {
                    // TODO: figure out what to do with this
                    // should i eval the astnode and return the object as an unbounded var?
                }
            }
            for (AstNode node : klassObject.getMethods()) {
                // TODO: figure out what to do with this
                // should i eval the astnode and return the object as an unbounded functuin?
                System.out.println(node);
            }
        }
        return null;
    }
}
