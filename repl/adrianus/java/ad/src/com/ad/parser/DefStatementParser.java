package com.ad.parser;

import com.ad.ast.AstNode;

public class DefStatementParser implements PrefixParseInterface {
    private ParserOLD parserOLD;

    public DefStatementParser(ParserOLD parserOLD) {
        this.parserOLD = parserOLD;
    }

    @Override
    public AstNode parse() {
        return parserOLD.parseDefStatement();
    }
}
