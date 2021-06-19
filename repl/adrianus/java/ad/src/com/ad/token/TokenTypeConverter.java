package com.ad.token;

import java.util.HashMap;

public class TokenTypeConverter {
	public static HashMap<TokenTypeEnum, String> tokenTypeMap = new HashMap<TokenTypeEnum, String>() {
		private static final long serialVersionUID = 1L;
		{
			put(TokenTypeEnum.ILLEGAL, "ILLEGAL");
			put(TokenTypeEnum.ASSIGN, "ASSIGN");
			put(TokenTypeEnum.IDENT, "IDENT");
			put(TokenTypeEnum.LET, "LET");
			put(TokenTypeEnum.RETURN, "RETURN");
			put(TokenTypeEnum.FUNCTION, "FUNCTION");
			put(TokenTypeEnum.INT, "INT");
			put(TokenTypeEnum.IF, "IF");
			put(TokenTypeEnum.ELSE, "ELSE");
			put(TokenTypeEnum.TRUE, "TRUE");
			put(TokenTypeEnum.FALSE, "FALSE");
			put(TokenTypeEnum.WHILE, "WHILE");
			put(TokenTypeEnum.COMMA, "COMMA");
			put(TokenTypeEnum.SEMICOLON, "SEMICOLON");
			put(TokenTypeEnum.LT, "LT");
			put(TokenTypeEnum.GT, "GT");
			put(TokenTypeEnum.LT, "LTE");
			put(TokenTypeEnum.GT, "GTE");
			put(TokenTypeEnum.EQ, "EQ");
			put(TokenTypeEnum.NOT_EQ, "NOT_EQ");
			put(TokenTypeEnum.PLUS, "PLUS");
			put(TokenTypeEnum.MINUS, "MINUS");
			put(TokenTypeEnum.SLASH, "SLASH");
			put(TokenTypeEnum.ASTERISK, "ASTERISK");
			put(TokenTypeEnum.LPAREN, "LPAREN");
			put(TokenTypeEnum.RPAREN, "RPAREN");
			put(TokenTypeEnum.LBRACE, "LBRACE");
			put(TokenTypeEnum.RBRACE, "RBRACE");
			put(TokenTypeEnum.BANG, "BANG");
			put(TokenTypeEnum.EOF, "EOF");
		}
	};
	
	public static String convertToString(TokenTypeEnum tte) {
		return tokenTypeMap.get(tte);
	}
}
