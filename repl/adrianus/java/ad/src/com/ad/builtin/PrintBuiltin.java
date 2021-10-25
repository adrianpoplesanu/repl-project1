package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;

import java.util.ArrayList;

public class PrintBuiltin implements BuiltinInterface {
    @Override
    public AdObject call(ArrayList<AdObject> args, Environment env) {
        AdObject obj = args.get(0);
        System.out.println(obj.inspect());
        return null;
    }
}
