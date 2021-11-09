package com.ad.parser;

import com.ad.ast.AstNode;

public class AssignStatementParser implements InfixParseInterface {
    private Parser parser;

    public AssignStatementParser(Parser parser) {
        this.parser = parser;
    }

    @Override
    public AstNode parse(AstNode node) {
        return null;
    }
}
