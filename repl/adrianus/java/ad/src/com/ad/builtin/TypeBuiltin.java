package com.ad.builtin;

import java.util.ArrayList;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;

public class TypeBuiltin implements BuiltinInterface {

	@Override
	public AdObject call(ArrayList<AdObject> args, Environment env) {
		AdStringObject type = new AdStringObject();
		switch(args.get(0).getType()) {
		case INT:
			type.setValue("INT");
			break;
		case BOOLEAN:
			type.setValue("BOOLEAN");
			break;
		case STRING:
			type.setValue("STRING");
			break;
		default:
			type.setValue("UNKNOWN_TYPE");
			break;
		}
		return type;
	}

}