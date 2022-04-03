package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdFileObject;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;

import java.util.List;

public class IOFileBuiltin implements BuiltinInterface {
    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        AdStringObject filenameObject = (AdStringObject) args.get(0);
        AdStringObject operatorObject = (AdStringObject) args.get(1);
        return new AdFileObject(filenameObject.getValue(), operatorObject.getValue());
    }
}
