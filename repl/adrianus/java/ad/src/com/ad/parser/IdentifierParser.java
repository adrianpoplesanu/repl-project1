package com.ad.parser;

import com.ad.ast.AstNode;

public class IdentifierParser implements PrefixParseInterface {
	private ParserOLD parserOLD;

	public IdentifierParser(ParserOLD parserOLD) {
		this.parserOLD = parserOLD;
	}

	@Override
	public AstNode parse() {
		return parserOLD.parseIdentifier();
	}
}
