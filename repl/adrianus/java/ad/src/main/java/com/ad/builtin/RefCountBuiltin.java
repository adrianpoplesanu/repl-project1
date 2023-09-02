package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;

import java.util.List;

public class RefCountBuiltin extends AbstractBuiltin {

    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        return new AdStringObject("not available in Java implementation");
    }
}
