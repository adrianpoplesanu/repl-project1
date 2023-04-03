package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;
import com.ad.objects.AdThreadObject;

import java.util.List;

public class ThreadBuiltin extends AbstractBuiltin {
    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        AdStringObject nameArgument = (AdStringObject) args.get(0);
        return new AdThreadObject(nameArgument.getValue());
    }
}
