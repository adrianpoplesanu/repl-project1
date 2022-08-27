package com.ad.builtin;

import java.util.List;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;

public interface BuiltinInterface {
	AdObject call(List<AdObject> args, Environment env);
}
