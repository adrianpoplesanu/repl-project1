package com.ad.parser;

import com.ad.ast.AstNode;

public class IfExpressionParser implements PrefixParseInterface {
	private Parser parser;

	public IfExpressionParser(Parser parser) {
		this.parser = parser;
	}

	@Override
	public AstNode parse() {
		return parser.parseIfExpression();
	}

}
