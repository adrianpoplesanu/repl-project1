package com.ad.builtin;

import java.util.ArrayList;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;

public interface BuiltinInterface {
	AdObject call(ArrayList<AdObject> args, Environment env);
}
