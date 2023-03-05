package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;

import java.util.List;

public class ImportBuiltin extends AbstractBuiltin {

    public ImportBuiltin() {
        super();
        acceptedParameterSize.add(1);
    }

    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        return null;
    }
}
