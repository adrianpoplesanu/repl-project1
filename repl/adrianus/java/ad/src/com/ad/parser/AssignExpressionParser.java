package com.ad.parser;

import com.ad.ast.AstNode;

public class AssignExpressionParser implements InfixParseInterface {
    private ParserOLD parserOLD;

    public AssignExpressionParser(ParserOLD parserOLD) {
        this.parserOLD = parserOLD;
    }

    @Override
    public AstNode parse(AstNode node) {
        return parserOLD.parseAssignExpression(node);
    }
}
