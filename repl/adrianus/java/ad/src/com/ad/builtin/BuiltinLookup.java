package com.ad.builtin;

import java.util.HashMap;

import com.ad.objects.AdBuiltinObject;

public class BuiltinLookup {
	public static HashMap<String, AdBuiltinObject> builtinMap = new HashMap<String, AdBuiltinObject>() {
		private static final long serialVersionUID = 1L;
		{
			put("type", new AdBuiltinObject(new TypeBuiltin()));
			put("print", new AdBuiltinObject(new PrintBuiltin()));
			put("context", new AdBuiltinObject(new ContextBuiltin()));
			put("len", new AdBuiltinObject(new LenBuiltin()));
		}
	};
}
