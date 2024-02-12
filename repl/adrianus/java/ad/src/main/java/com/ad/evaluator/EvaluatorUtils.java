package com.ad.evaluator;

import com.ad.ast.AstIdentifier;
import com.ad.ast.AstNode;
import com.ad.objects.AdIntegerObject;
import com.ad.objects.AdListObject;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import static com.ad.evaluator.Evaluator.NULLOBJECT;

public class EvaluatorUtils {
    public static AdObject dispatchMemberAccessPerObjectType(AdObject target, AstIdentifier member, List<AdObject> arguments) {
        switch(target.getType()) {
            case STRING:
                AdStringObject stringObject = (AdStringObject) target;
                if (member.getValue().equals("size")) {
                    return new AdIntegerObject(stringObject.getValue().length());
                }
                if (member.getValue().equals("length")) {
                    return new AdIntegerObject(stringObject.getValue().length());
                }
                if (member.getValue().equals("lower")) {
                    return new AdStringObject(stringObject.getValue().toLowerCase());
                }
                if (member.getValue().equals("upper")) {
                    return new AdStringObject(stringObject.getValue().toUpperCase());
                }
                if (member.getValue().equals("split")) {
                    // TODO: check params
                    List<AdObject> elements = new ArrayList<>();
                    AdListObject result = new AdListObject();
                    String delim = " ";
                    if (arguments.size() > 0) {
                        delim = ((AdStringObject)arguments.get(0)).getValue();
                    }
                    Arrays.stream(stringObject.getValue().split(delim))
                            .forEach(word -> elements.add(new AdStringObject(word)));
                    result.setElements(elements);
                    return result;
                }
                break;
            case LIST:
                AdListObject listObject = (AdListObject) target;
                if (member.getValue().equals("size")) {
                    return new AdIntegerObject(listObject.getElements().size());
                }
                if (member.getValue().equals("length")) {
                    return new AdIntegerObject(listObject.getElements().size());
                }
                break;
            default:
                return NULLOBJECT;
        }
        return NULLOBJECT;
    }
}
