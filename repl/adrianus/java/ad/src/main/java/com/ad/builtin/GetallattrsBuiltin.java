package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.*;

import java.util.ArrayList;
import java.util.List;
import java.util.stream.Collectors;

public class GetallattrsBuiltin extends AbstractBuiltin {
    public GetallattrsBuiltin() {
        super();
        acceptedParameterSize.add(1);
    }

    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        if (args.size() == 1) {
            AdObject target = args.get(0);
            List<AdObject> elements = new ArrayList<>();
            if (target.getType() != ObjectTypeEnum.INSTANCE) {
                return new AdErrorObject("getallattrs can only be called on a class instance");
            }
            AdClassInstance klassInstance = (AdClassInstance) target;
            klassInstance.getEnvironment().populateGetattrs().stream().forEach(attrName -> elements.add(new AdStringObject(attrName)));
            AdListObject result = new AdListObject(elements);
            return result;
        }
        return null;
    }
}
