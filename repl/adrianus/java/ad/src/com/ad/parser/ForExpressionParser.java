package com.ad.parser;

import com.ad.ast.AstNode;

public class ForExpressionParser implements PrefixParseInterface {
    private ParserOLD parserOLD;

    public ForExpressionParser(ParserOLD parserOLD) {
        this.parserOLD = parserOLD;
    }

    @Override
    public AstNode parse() {
        return parserOLD.parseForExpression();
    }
}
