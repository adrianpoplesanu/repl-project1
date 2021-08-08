package com.ad.parser;

import com.ad.ast.AstNode;

public class PrefixExpressionParser implements PrefixParseInterface {
	private Parser parser;

	public PrefixExpressionParser(Parser parser) {
		this.parser = parser;
	}

	@Override
	public AstNode parse() {
		return parser.parsePrefixExpression();
	}
}
