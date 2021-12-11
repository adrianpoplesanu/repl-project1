package com.ad.parser;

import com.ad.ast.AstNode;

public class FunctionLiteralParser implements PrefixParseInterface {
	private ParserOLD parserOLD;
	
	public FunctionLiteralParser(ParserOLD parserOLD) {
		this.parserOLD = parserOLD;
	}

	@Override
	public AstNode parse() {
		return parserOLD.parseFunctionLiteral();
	}
}
