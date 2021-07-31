package com.ad.parser;

import com.ad.ast.AstInteger;
import com.ad.ast.AstNode;

public class IntegerLiteralParser implements PrefixParseInterface {
	private Parser parser;

	public IntegerLiteralParser(Parser parser) {
		this.parser = parser;
	}

	@Override
	public AstNode parse() {
		String value = parser.getCurrentToken().getLiteral();
		return new AstInteger(parser.getCurrentToken(), Integer.parseInt(value));
	}
}
