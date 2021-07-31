package com.ad.parser;

import com.ad.ast.AstNode;
import com.ad.ast.AstPrefixExpression;

public class PrefixExpressionParser implements PrefixParseInterface {
	private Parser parser;

	public PrefixExpressionParser(Parser parser) {
		this.parser = parser;
	}

	@Override
	public AstNode parse() {
		AstPrefixExpression expr = new AstPrefixExpression(this.parser.getCurrentToken(),
				this.parser.getCurrentToken().getLiteral());
		this.parser.nextToken();
		expr.setRight(this.parser.parseExpression(PrecedenceTypeEnum.PREFIX));
		return expr;
	}
}
