package com.ad.parser;

import com.ad.ast.AstNode;

public class IntegerLiteralParser implements PrefixParseInterface {
	private ParserOLD parserOLD;

	public IntegerLiteralParser(ParserOLD parserOLD) {
		this.parserOLD = parserOLD;
	}

	@Override
	public AstNode parse() {
		return parserOLD.parseInteger();
	}
}
