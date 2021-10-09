package com.ad.parser;

import com.ad.ast.AstNode;

public class CallExpressionParser implements InfixParseInterface {
	private Parser parser;
	
	public CallExpressionParser(Parser parser) {
		this.parser = parser;
	}

	@Override
	public AstNode parse(AstNode node) {
		return parser.parseCallExpression(node);
	}
}
