package com.ad.token;

import java.util.HashMap;

public class TokenKeywordConverter {
	public static HashMap<String, TokenTypeEnum> keywords = new HashMap<String, TokenTypeEnum> () {
		private static final long serialVersionUID = 1L;
	    {
		    put("let", TokenTypeEnum.LET);
		    put("return", TokenTypeEnum.RETURN);
		    put("func", TokenTypeEnum.FUNC);
		    put("if", TokenTypeEnum.IF);
		    put("else", TokenTypeEnum.ELSE);
		    put("true", TokenTypeEnum.TRUE);
		    put("false", TokenTypeEnum.FALSE);
		    put("while", TokenTypeEnum.WHILE);
		    put("def", TokenTypeEnum.DEF);
			put("fun", TokenTypeEnum.FUN);
			put("function", TokenTypeEnum.FUNCTION);
		    put("null", TokenTypeEnum.NULL);
		    put("class", TokenTypeEnum.CLASS);
		    put("this", TokenTypeEnum.THIS);
		    put("and", TokenTypeEnum.AND);
			put("or", TokenTypeEnum.OR);
			put("for", TokenTypeEnum.FOR);
			put("break", TokenTypeEnum.BREAK);
			put("continue", TokenTypeEnum.CONTINUE);
			put("super", TokenTypeEnum.SUPER);
	    }
	};
	
	public static TokenTypeEnum convertToKeyword(String ident) {
		return keywords.get(ident);
	}
}
