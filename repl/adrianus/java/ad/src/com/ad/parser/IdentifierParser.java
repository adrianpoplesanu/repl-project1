package com.ad.parser;

import com.ad.ast.AstNode;

public class IdentifierParser implements PrefixParseInterface {
	private Parser parser;

	public IdentifierParser(Parser parser) {
		this.parser = parser;
	}

	@Override
	public AstNode parse() {
		return parser.parseIdentifier();
	}
}
