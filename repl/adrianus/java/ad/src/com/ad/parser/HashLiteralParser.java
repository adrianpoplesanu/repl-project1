package com.ad.parser;

import com.ad.ast.AstNode;

public class HashLiteralParser implements PrefixParseInterface {
    private Parser parser;

    public HashLiteralParser(Parser parser) {
        this.parser = parser;
    }

    @Override
    public AstNode parse() {
        return parser.parseHashLiteral();
    }
}
