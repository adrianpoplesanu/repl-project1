package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;

import java.util.List;

public class PrintlnBuiltin extends AbstractBuiltin {
    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        AdObject obj = args.get(0);
        if (obj != null) {
            System.out.println(obj.inspect());
        }
        return null;
    }
}
