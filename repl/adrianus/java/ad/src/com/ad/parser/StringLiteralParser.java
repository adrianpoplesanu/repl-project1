package com.ad.parser;

import com.ad.ast.AstNode;

public class StringLiteralParser implements PrefixParseInterface {
	private Parser parser;

	public StringLiteralParser(Parser parser) {
		this.parser = parser;
	}
	
	@Override
	public AstNode parse() {
		return parser.parseStringLiteral();
	}

}
