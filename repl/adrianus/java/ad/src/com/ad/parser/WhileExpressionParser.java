package com.ad.parser;

import com.ad.ast.AstNode;

public class WhileExpressionParser implements PrefixParseInterface {
	private Parser parser;
	
	public WhileExpressionParser(Parser parser) {
		this.parser = parser;
	}

	@Override
	public AstNode parse() {
		return parser.parseWhileExpression();
	}

}
