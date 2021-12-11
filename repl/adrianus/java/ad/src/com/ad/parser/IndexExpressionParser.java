package com.ad.parser;

import com.ad.ast.AstNode;

public class IndexExpressionParser implements InfixParseInterface {
    private ParserOLD parserOLD;

    public IndexExpressionParser(ParserOLD parserOLD) {
        this.parserOLD = parserOLD;
    }

    @Override
    public AstNode parse(AstNode node) {
        return parserOLD.parseIndexExpression(node);
    }
}
