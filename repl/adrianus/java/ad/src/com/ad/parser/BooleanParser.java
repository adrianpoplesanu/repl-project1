package com.ad.parser;

import com.ad.ast.AstNode;

public class BooleanParser implements PrefixParseInterface {
	private Parser parser;

	public BooleanParser(Parser parser) {
		this.parser = parser;
	}

	@Override
	public AstNode parse() {
		return parser.parseBoolean();
	}
}
