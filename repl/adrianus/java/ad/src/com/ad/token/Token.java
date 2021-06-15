package com.ad.token;

public class Token {
	private TokenTypeEnum type;
	private String literal;
	
	public Token() {
		type = TokenTypeEnum.ILLEGAL;
		literal = "";
	}
	
	public Token(TokenTypeEnum t, String l) {
		type = t;
		literal = l;
	}
	
	public String toString() {
		return "Token [type: " + TokenTypeConverter.convertToString(type) + ", literal: " + literal + "]";
	}

	public TokenTypeEnum getType() {
		return type;
	}

	public void setType(TokenTypeEnum type) {
		this.type = type;
	}

	public String getLiteral() {
		return literal;
	}

	public void setLiteral(String literal) {
		this.literal = literal;
	}
}
