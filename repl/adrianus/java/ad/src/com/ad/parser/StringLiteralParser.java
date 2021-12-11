package com.ad.parser;

import com.ad.ast.AstNode;

public class StringLiteralParser implements PrefixParseInterface {
	private ParserOLD parserOLD;

	public StringLiteralParser(ParserOLD parserOLD) {
		this.parserOLD = parserOLD;
	}
	
	@Override
	public AstNode parse() {
		return parserOLD.parseStringLiteral();
	}

}
