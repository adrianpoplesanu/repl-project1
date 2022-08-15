package com.ad.objects;

import java.util.HashMap;

public class ObjectTypeConverter {
    public static HashMap<ObjectTypeEnum, String> objectTypeMap = new HashMap<ObjectTypeEnum, String>() {
		private static final long serialVersionUID = 1L;
		{
			put(ObjectTypeEnum.NULL, "NullObject");
			put(ObjectTypeEnum.INT, "IntObject");
			put(ObjectTypeEnum.BOOLEAN, "BooleanObject");
			put(ObjectTypeEnum.STRING, "StringObject");
			put(ObjectTypeEnum.RETURN_VALUE, "ReturnValueObject");
			put(ObjectTypeEnum.BREAK, "BreakObject");
			put(ObjectTypeEnum.FUNCTION, "FunctionObject");
			put(ObjectTypeEnum.ERROR, "ErrorObject");
			put(ObjectTypeEnum.BUILTIN, "BuiltinObject");
			put(ObjectTypeEnum.LIST, "ListObject");
			put(ObjectTypeEnum.HASH, "HashObject");
			put(ObjectTypeEnum.CLASS, "ClassObject");
			put(ObjectTypeEnum.INSTANCE, "InstanceObject");
			put(ObjectTypeEnum.FILE, "FileObject");
			put(ObjectTypeEnum.SOCKET, "SocketObject");
		}
    };
    
    public static String convertToString(ObjectTypeEnum ote) {
    	return objectTypeMap.get(ote);
    }
}
