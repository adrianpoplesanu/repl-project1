package com.ad.parser;

import com.ad.ast.AstNode;

public class IfExpressionParser implements PrefixParseInterface {
	private ParserOLD parserOLD;

	public IfExpressionParser(ParserOLD parserOLD) {
		this.parserOLD = parserOLD;
	}

	@Override
	public AstNode parse() {
		return parserOLD.parseIfExpression();
	}

}
