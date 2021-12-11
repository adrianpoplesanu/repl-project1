package com.ad.parser;

import com.ad.ast.AstNode;

public class InfixExpressionParser implements InfixParseInterface {
	private ParserOLD parserOLD;

	public InfixExpressionParser(ParserOLD parserOLD) {
		this.parserOLD = parserOLD;
	}

	@Override
	public AstNode parse(AstNode left) {
		return parserOLD.parseInfixExpression(left);
	}
}
