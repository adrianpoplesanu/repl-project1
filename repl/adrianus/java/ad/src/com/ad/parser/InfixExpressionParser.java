package com.ad.parser;

import com.ad.ast.AstNode;

public class InfixExpressionParser implements InfixParseInterface {
	private Parser parser;

	public InfixExpressionParser(Parser parser) {
		this.parser = parser;
	}

	@Override
	public AstNode parse(AstNode left) {
		return parser.parseInfixExpression(left);
	}
}
