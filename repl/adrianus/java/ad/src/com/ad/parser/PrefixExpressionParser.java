package com.ad.parser;

import com.ad.ast.AstNode;

public class PrefixExpressionParser implements PrefixParseInterface {
	private ParserOLD parserOLD;

	public PrefixExpressionParser(ParserOLD parserOLD) {
		this.parserOLD = parserOLD;
	}

	@Override
	public AstNode parse() {
		return parserOLD.parsePrefixExpression();
	}
}
