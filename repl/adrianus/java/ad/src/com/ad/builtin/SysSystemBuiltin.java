package com.ad.builtin;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;

import java.util.List;

public class SysSystemBuiltin extends AbstractBuiltin {
    @Override
    public AdObject call(List<AdObject> args, Environment env) {
        // run a system command and print the output...
        return new AdStringObject("output of the sys command");
    }
}
