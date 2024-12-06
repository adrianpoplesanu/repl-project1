package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.*;

import java.util.List;

public class SetattrBuiltin extends AbstractBuiltin {
    public SetattrBuiltin() {
        super();
        acceptedParameterSize.add(3);
    }

    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        AdObject target = args.get(0);
        if (target.getType() == ObjectTypeEnum.INSTANCE) {
            AdStringObject name = (AdStringObject) args.get(1);
            AdObject value = args.get(2);
            if (value.getType() == ObjectTypeEnum.FUNCTION) {
                AdFunctionObject functionObject = (AdFunctionObject) value;
                functionObject.setEnv(((AdClassInstance)target).getEnvironment());
                ((AdClassInstance) target).getEnvironment().store.put(name.getValue(), value);
            } else {
                ((AdClassInstance) target).getEnvironment().store.put(name.getValue(), value);
            }
        }
        return null;
    }
}
