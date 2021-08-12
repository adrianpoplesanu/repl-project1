package com.ad.parser;

import com.ad.ast.AstNode;

public class FunctionLiteralParser implements PrefixParseInterface {
	private Parser parser;
	
	public FunctionLiteralParser(Parser parser) {
		this.parser = parser;
	}

	@Override
	public AstNode parse() {
		return parser.parseFunctionLiteral();
	}
}
