package com.ad.parser;

import com.ad.ast.AstNode;

public class ListLiteralParser implements PrefixParseInterface {
    private ParserOLD parserOLD;

    public ListLiteralParser(ParserOLD parserOLD) {
        this.parserOLD = parserOLD;
    }

    @Override
    public AstNode parse() {
        return parserOLD.parseListLiteral();
    }
}
