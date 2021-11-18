package com.ad.parser;

import com.ad.ast.AstNode;

public class ListLiteralParser implements PrefixParseInterface {
    private Parser parser;

    public ListLiteralParser(Parser parser) {
        this.parser = parser;
    }

    @Override
    public AstNode parse() {
        return parser.parseListLiteral();
    }
}
