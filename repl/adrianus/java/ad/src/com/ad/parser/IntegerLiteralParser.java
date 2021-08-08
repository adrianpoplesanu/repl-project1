package com.ad.parser;

import com.ad.ast.AstNode;

public class IntegerLiteralParser implements PrefixParseInterface {
	private Parser parser;

	public IntegerLiteralParser(Parser parser) {
		this.parser = parser;
	}

	@Override
	public AstNode parse() {
		return parser.parseInteger();
	}
}
