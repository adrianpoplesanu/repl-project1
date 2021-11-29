package com.ad.parser;

import com.ad.ast.AstNode;

public class IndexExpressionParser implements InfixParseInterface {
    private Parser parser;

    public IndexExpressionParser(Parser parser) {
        this.parser = parser;
    }

    @Override
    public AstNode parse(AstNode node) {
        return parser.parseIndexExpression(node);
    }
}
