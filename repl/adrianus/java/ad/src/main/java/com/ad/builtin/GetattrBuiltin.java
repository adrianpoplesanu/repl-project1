package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;

import java.util.List;

public class GetattrBuiltin extends AbstractBuiltin {
    public GetattrBuiltin() {
        super();
        acceptedParameterSize.add(2);
    }

    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        if (args.size() == 1) {

        }
        return null;
    }
}
