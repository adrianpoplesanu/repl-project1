package com.ad.parser;

import java.util.HashMap;

import com.ad.token.TokenTypeEnum;

public class TokenPrecedenceConverter {
	public static HashMap<TokenTypeEnum, PrecedenceTypeEnum> precedenceMap = new HashMap<TokenTypeEnum, PrecedenceTypeEnum>() {
		private static final long serialVersionUID = 1L;
		{
			put(TokenTypeEnum.EQ, PrecedenceTypeEnum.EQUALS);
			put(TokenTypeEnum.NOT_EQ, PrecedenceTypeEnum.EQUALS);
			put(TokenTypeEnum.LT, PrecedenceTypeEnum.LESSGREATER);
			put(TokenTypeEnum.GT, PrecedenceTypeEnum.LESSGREATER);
			put(TokenTypeEnum.LTE, PrecedenceTypeEnum.LESSGREATER);
			put(TokenTypeEnum.GTE, PrecedenceTypeEnum.LESSGREATER);
			//put(TokenTypeEnum.AND, PrecedenceTypeEnum.CONDITIONAL);
			//put(TokenTypeEnum.OR, PrecedenceTypeEnum.CONDITIONAL);
			put(TokenTypeEnum.PLUS, PrecedenceTypeEnum.SUM);
			put(TokenTypeEnum.MINUS, PrecedenceTypeEnum.SUM);
			put(TokenTypeEnum.ASTERISK, PrecedenceTypeEnum.PRODUCT);
			put(TokenTypeEnum.SLASH, PrecedenceTypeEnum.PRODUCT);
			put(TokenTypeEnum.LPAREN, PrecedenceTypeEnum.CALL);
		}
	};
	
	public static PrecedenceTypeEnum getPrecedence(TokenTypeEnum tte) {
		return precedenceMap.get(tte);
	}
}
