package com.ad.parser;

import com.ad.ast.AstBoolean;
import com.ad.ast.AstNode;
import com.ad.token.TokenTypeEnum;

public class BooleanParser implements PrefixParseInterface {
	private Parser parser;

	public BooleanParser(Parser parser) {
		this.parser = parser;
	}

	@Override
	public AstNode parse() {
		return new AstBoolean(parser.getCurrentToken(), parser.currentTokenIs(TokenTypeEnum.TRUE));
	}
}
