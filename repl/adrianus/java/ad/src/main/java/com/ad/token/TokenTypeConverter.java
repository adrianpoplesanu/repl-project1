package com.ad.token;

import java.util.HashMap;

public class TokenTypeConverter {
	public static HashMap<TokenTypeEnum, String> tokenTypeMap = new HashMap<TokenTypeEnum, String>() {
		private static final long serialVersionUID = 1L;
		{
			put(TokenTypeEnum.ILLEGAL, "ILLEGAL");
			put(TokenTypeEnum.ASSIGN, "ASSIGN");
			put(TokenTypeEnum.EOF, "EOF");
			put(TokenTypeEnum.LET, "LET");
			put(TokenTypeEnum.RETURN, "RETURN");
			put(TokenTypeEnum.BREAK, "BREAK");
			put(TokenTypeEnum.CONTINUE, "CONTINUE");
			put(TokenTypeEnum.WHILE, "WHILE");
			put(TokenTypeEnum.FOR, "FOR");
			put(TokenTypeEnum.FOREACH, "FOREACH");
			put(TokenTypeEnum.FUNCTION, "FUNCTION");
			put(TokenTypeEnum.DEF, "DEF");
			put(TokenTypeEnum.IDENT, "IDENT");
			put(TokenTypeEnum.INT, "INT");
			put(TokenTypeEnum.FLOAT, "FLOAT");
			put(TokenTypeEnum.PLUS, "PLUS");
			put(TokenTypeEnum.MINUS, "MINUS");
			put(TokenTypeEnum.SLASH, "SLASH");
			put(TokenTypeEnum.ASTERISK, "ASTERISK");
			put(TokenTypeEnum.BANG, "BANG");
			put(TokenTypeEnum.COMMA, "COMMA");
			put(TokenTypeEnum.SEMICOLON, "SEMICOLON");
			put(TokenTypeEnum.LPAREN, "LPAREN");
			put(TokenTypeEnum.RPAREN, "RPAREN");
			put(TokenTypeEnum.LBRACE, "LBRACE");
			put(TokenTypeEnum.RBRACE, "RBRACE");
			put(TokenTypeEnum.LT, "LT");
			put(TokenTypeEnum.GT, "GT");
			put(TokenTypeEnum.LTE, "LTE");
			put(TokenTypeEnum.GTE, "GTE");
			put(TokenTypeEnum.EQ, "EQ");
			put(TokenTypeEnum.NOT_EQ, "NOT_EQ");
			put(TokenTypeEnum.IF, "IF");
			put(TokenTypeEnum.ELSE, "ELSE");
			put(TokenTypeEnum.TRUE, "TRUE");
			put(TokenTypeEnum.FALSE, "FALSE");
			put(TokenTypeEnum.STRING, "STRING");
			put(TokenTypeEnum.AND, "AND");
			put(TokenTypeEnum.OR, "OR");
			put(TokenTypeEnum.LBRACKET, "LBRACKET");
			put(TokenTypeEnum.RBRACKET, "RBRACKET");
			put(TokenTypeEnum.COLON, "COLON");
			put(TokenTypeEnum.DOT, "DOT");
			put(TokenTypeEnum.CLASS, "CLASS");
			put(TokenTypeEnum.THIS, "THIS");
			put(TokenTypeEnum.START_COMMENT, "START_COMMENT");
			put(TokenTypeEnum.END_COMMENT, "END_COMMENT");
			put(TokenTypeEnum.PLUSPLUS, "INCREMENT_OPERATOR");
			put(TokenTypeEnum.MINUSMINUS, "DECREMENT_OPERATOR");
			put(TokenTypeEnum.NULL, "NULL");
		}
	};
	
	public static String convertToString(TokenTypeEnum tte) {
		return tokenTypeMap.get(tte);
	}
}