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
		}
    };
    
    public static String convertToString(ObjectTypeEnum ote) {
    	return objectTypeMap.get(ote);
    }
}
