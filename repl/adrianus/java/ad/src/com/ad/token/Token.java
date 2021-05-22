package com.ad.token;

public class Token {
	private TokenTypeEnum type;
	private String literal;
	
	Token() {
		type = TokenTypeEnum.ILLEGAL;
		literal = "";
	}
	
	Token(TokenTypeEnum t, String l) {
		type = t;
		literal = l;
	}
	
	String ToString() {
		return "Token [type: " + TokenTypeConverter.convertToString(type) + ", literal: " + literal + "]";
	}
}
