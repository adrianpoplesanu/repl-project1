package com.ad.parser;

import com.ad.ast.AstNode;

public class AssignStatementParser implements InfixParseInterface {
    private ParserOLD parserOLD;

    public AssignStatementParser(ParserOLD parserOLD) {
        this.parserOLD = parserOLD;
    }

    @Override
    public AstNode parse(AstNode node) {
        return null;
    }
}
