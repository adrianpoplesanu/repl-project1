package com.ad.parser;

import com.ad.ast.AstNode;

public class DefStatementParser implements PrefixParseInterface {
    private Parser parser;

    public DefStatementParser(Parser parser) {
        this.parser = parser;
    }

    @Override
    public AstNode parse() {
        return parser.parseDefStatement();
    }
}
