package com.ad.parser;

import com.ad.ast.AstNode;

public class GroupExpressionParser implements PrefixParseInterface {
	private Parser parser;

	public GroupExpressionParser(Parser parser) {
		this.parser = parser;
	}

	@Override
	public AstNode parse() {
		return parser.parseGroupedExpression();
	}

}
