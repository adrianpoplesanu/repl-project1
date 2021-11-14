package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;

import java.util.ArrayList;

public class ContextBuiltin implements BuiltinInterface {

    @Override
    public AdObject call(ArrayList<AdObject> args, Environment env) {
        AdStringObject obj = new AdStringObject(env.toString());
        return obj;
    }
}
