package com.ad.parser;

import com.ad.ast.AstNode;

public class WhileExpressionParser implements PrefixParseInterface {
	private ParserOLD parserOLD;
	
	public WhileExpressionParser(ParserOLD parserOLD) {
		this.parserOLD = parserOLD;
	}

	@Override
	public AstNode parse() {
		return parserOLD.parseWhileExpression();
	}

}
