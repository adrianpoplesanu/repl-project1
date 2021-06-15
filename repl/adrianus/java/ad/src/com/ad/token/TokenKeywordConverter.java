package com.ad.token;

import java.util.HashMap;

public class TokenKeywordConverter {
	static HashMap<String, TokenTypeEnum> keywords = new HashMap<String, TokenTypeEnum> () {
		private static final long serialVersionUID = 1L;
	{
		put("let", TokenTypeEnum.LET);
		put("return", TokenTypeEnum.LET);
		put("if", TokenTypeEnum.LET);
		put("else", TokenTypeEnum.LET);
		put("true", TokenTypeEnum.LET);
		put("false", TokenTypeEnum.LET);
	}};
}
