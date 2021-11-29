package com.ad.parser;

import com.ad.ast.AstNode;

public class AssignExpressionParser implements InfixParseInterface {
    private Parser parser;

    public AssignExpressionParser(Parser parser) {
        this.parser = parser;
    }

    @Override
    public AstNode parse(AstNode node) {
        return parser.parseAssignExpression(node);
    }
}
