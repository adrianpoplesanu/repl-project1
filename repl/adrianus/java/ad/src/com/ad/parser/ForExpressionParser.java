package com.ad.parser;

import com.ad.ast.AstNode;

public class ForExpressionParser implements PrefixParseInterface {
    private Parser parser;

    public ForExpressionParser(Parser parser) {
        this.parser = parser;
    }

    @Override
    public AstNode parse() {
        return parser.parseForExpression();
    }
}
