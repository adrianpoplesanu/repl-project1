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
			put("println", new AdBuiltinObject(new PrintlnBuiltin()));
			put("ref_count", new AdBuiltinObject(new RefCountBuiltin()));
			put("type", new AdBuiltinObject(new TypeBuiltin()));
			put("__append", new AdBuiltinObject(new TypeBuiltin())); // TODO: update this
			put("__pop", new AdBuiltinObject(new TypeBuiltin())); // TODO: update this
			put("__remove", new AdBuiltinObject(new TypeBuiltin())); // TODO: update this
			put("__lower", new AdBuiltinObject(new TypeBuiltin())); // TODO: update this
			put("__upper", new AdBuiltinObject(new TypeBuiltin())); // TODO: update this
			put("__context", new AdBuiltinObject(new ContextBuiltin()));
			put("__locals", new AdBuiltinObject(new LocalsBuiltin()));
			put("__iofile", new AdBuiltinObject(new IOFileBuiltin()));
			put("__syssystem", new AdBuiltinObject(new SysSystemBuiltin()));
			put("__iosocket", new AdBuiltinObject(new IOSocketBuiltin()));
			put("eval", new AdBuiltinObject(new EvalBuiltin()));
			put("__first", new AdBuiltinObject(new ContextBuiltin())); // TODO: update this
			put("__last", new AdBuiltinObject(new ContextBuiltin())); // TODO: update this
			put("map", new AdBuiltinObject(new ContextBuiltin())); // TODO: update this
			put("input", new AdBuiltinObject(new InputBuiltin()));
			put("list", new AdBuiltinObject(new ListBuiltin()));
			put("mat", new AdBuiltinObject(new MatBuiltin()));
			put("hash", new AdBuiltinObject(new ContextBuiltin())); // TODO: update this
			put("hasattr", new AdBuiltinObject(new ContextBuiltin())); // TODO: update this
			put("getattr", new AdBuiltinObject(new GetattrBuiltin()));
			put("setattr", new AdBuiltinObject(new SetattrBuiltin())); // TODO: update this
			put("getattrs", new AdBuiltinObject(new GetattrsBuiltin()));
			put("__thread", new AdBuiltinObject(new ThreadBuiltin()));
			put("import", new AdBuiltinObject(new ImportBuiltin()));
			put("sleep", new AdBuiltinObject(new SleepBuiltin()));
			// https://www.w3schools.com/python/python_ref_keywords.asp
			// https://www.w3schools.com/python/python_ref_functions.asp
		}
	};
}
