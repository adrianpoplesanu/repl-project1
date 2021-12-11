package com.ad.parser;

import com.ad.ast.AstNode;

public class BooleanParser implements PrefixParseInterface {
	private ParserOLD parserOLD;

	public BooleanParser(ParserOLD parserOLD) {
		this.parserOLD = parserOLD;
	}

	@Override
	public AstNode parse() {
		return parserOLD.parseBoolean();
	}
}
