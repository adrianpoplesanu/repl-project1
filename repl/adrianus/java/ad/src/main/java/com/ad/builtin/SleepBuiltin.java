package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdIntegerObject;
import com.ad.objects.AdObject;

import java.util.List;
import java.util.concurrent.TimeUnit;

public class SleepBuiltin extends AbstractBuiltin {
    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        try {
            TimeUnit.MILLISECONDS.sleep(((AdIntegerObject) args.get(0)).getValue());
        } catch (InterruptedException e) {
            throw new RuntimeException(e);
        }
        return null;
    }
}
