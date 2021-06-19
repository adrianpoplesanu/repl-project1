package com.ad.ast;

import java.util.HashMap;

public class AstNodeTypeConverter {
	public static HashMap<AstNodeTypeEnum, String> astNodeTypeMap = new HashMap<AstNodeTypeEnum, String>() {
		private static final long serialVersionUID = 1L;
		{
			put(AstNodeTypeEnum.PROGRAM, "Program");
			put(AstNodeTypeEnum.LET_STATEMENT, "LetStatement");
		}
	};
	
	public static String convertToString(AstNodeTypeEnum ante) {
		return astNodeTypeMap.get(ante);
	}
}
