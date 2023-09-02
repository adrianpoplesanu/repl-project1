package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;

import java.util.List;

public class ContextBuiltin extends AbstractBuiltin {

    public ContextBuiltin() {
        super();
        acceptedParameterSize.add(0);
    }

    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        //AdStringObject obj = new AdStringObject(env.toString());
        //AdObject obj = env.storeToAdHashObject();
        return null;
    }
}
