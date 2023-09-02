package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;

import java.util.List;

public class LocalsBuiltin extends AbstractBuiltin {
    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        AdObject obj = env.storeToAdHashObject();
        return obj;
    }
}
