package com.ad.builtin;

import java.util.HashMap;

import com.ad.objects.AdBuiltinObject;

public class BuiltinLookup {
	public static HashMap<String, AdBuiltinObject> builtinMap = new HashMap<String, AdBuiltinObject>() {
		private static final long serialVersionUID = 1L;
		{
			put("len", new AdBuiltinObject(new LenBuiltin()));
			put("exit", new AdBuiltinObject(new LenBuiltin()));
			put("print", new AdBuiltinObject(new PrintBuiltin()));
			put("ref_count", new AdBuiltinObject(null)); // this should display not available in Java implementation
			put("type", new AdBuiltinObject(new TypeBuiltin()));
			put("append", new AdBuiltinObject(new TypeBuiltin())); // TODO: update this
			put("pop", new AdBuiltinObject(new TypeBuiltin())); // TODO: update this
			put("remove", new AdBuiltinObject(new TypeBuiltin())); // TODO: update this
			put("lower", new AdBuiltinObject(new TypeBuiltin())); // TODO: update this
			put("upper", new AdBuiltinObject(new TypeBuiltin())); // TODO: update this
			put("context", new AdBuiltinObject(new ContextBuiltin()));
			put("__iofile", new AdBuiltinObject(new IOFileBuiltin())); // TODO: update this
			put("__syssystem", new AdBuiltinObject(new SysSystemBuiltin())); // TODO: update this
			put("__iosocket", new AdBuiltinObject(new IOSocketBuiltin())); // TODO: update this
			put("eval", new AdBuiltinObject(new ContextBuiltin())); // TODO: update this
			put("first", new AdBuiltinObject(new ContextBuiltin())); // TODO: update this
			put("last", new AdBuiltinObject(new ContextBuiltin())); // TODO: update this
			put("map", new AdBuiltinObject(new ContextBuiltin())); // TODO: update this
			put("input", new AdBuiltinObject(new InputBuiltin())); // TODO: update this
			put("list", new AdBuiltinObject(new ListBuiltin()));
			put("map", new AdBuiltinObject(new MapBuiltin()));
			put("hash", new AdBuiltinObject(new ContextBuiltin())); // TODO: update this
			// https://www.w3schools.com/python/python_ref_keywords.asp
			// https://www.w3schools.com/python/python_ref_functions.asp
		}
	};
}