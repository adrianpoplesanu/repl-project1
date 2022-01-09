package com.ad.token;

import java.util.HashMap;

public class TokenKeywordConverter {
	public static HashMap<String, TokenTypeEnum> keywords = new HashMap<String, TokenTypeEnum> () {
		private static final long serialVersionUID = 1L;
	    {
		    put("let", TokenTypeEnum.LET);
		    put("return", TokenTypeEnum.RETURN);
		    put("function", TokenTypeEnum.FUNCTION);
		    put("if", TokenTypeEnum.IF);
		    put("else", TokenTypeEnum.ELSE);
		    put("true", TokenTypeEnum.TRUE);
		    put("false", TokenTypeEnum.FALSE);
		    put("while", TokenTypeEnum.WHILE);
		    put("def", TokenTypeEnum.DEF);
		    put("null", TokenTypeEnum.NULL);
	    }
	};
	
	public static TokenTypeEnum convertToKeyword(String ident) {
		return keywords.get(ident);
	}
}
