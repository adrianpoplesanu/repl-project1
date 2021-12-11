package com.ad.parser;

import com.ad.ast.AstNode;

public class HashLiteralParser implements PrefixParseInterface {
    private ParserOLD parserOLD;

    public HashLiteralParser(ParserOLD parserOLD) {
        this.parserOLD = parserOLD;
    }

    @Override
    public AstNode parse() {
        return parserOLD.parseHashLiteral();
    }
}
