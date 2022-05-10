package com.ad.builtin;

import java.util.List;

import com.ad.environment.Environment;
import com.ad.objects.AdObject;
import com.ad.objects.AdStringObject;

public class TypeBuiltin implements BuiltinInterface {

	@Override
	public AdObject call(List<AdObject> args, Environment env) {
		AdStringObject type = new AdStringObject();
		switch(args.get(0).getType()) {
		case INT:
			type.setValue("INT");
			break;
		case FLOAT:
			type.setValue("FLOAT");
			break;
		case BOOLEAN:
			type.setValue("BOOLEAN");
			break;
		case STRING:
			type.setValue("STRING");
			break;
		case FUNCTION:
			type.setValue("FUNCTION");
			break;
		case LIST:
			type.setValue("LIST");
			break;
		case HASH:
			type.setValue("HASH");
			break;
		default:
			type.setValue("UNKNOWN_TYPE");
			break;
		}
		return type;
	}

}
