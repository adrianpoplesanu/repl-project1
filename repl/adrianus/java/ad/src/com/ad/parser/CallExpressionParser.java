package com.ad.parser;

import com.ad.ast.AstNode;

public class CallExpressionParser implements InfixParseInterface {
	private ParserOLD parserOLD;
	
	public CallExpressionParser(ParserOLD parserOLD) {
		this.parserOLD = parserOLD;
	}

	@Override
	public AstNode parse(AstNode node) {
		return parserOLD.parseCallExpression(node);
	}
}
