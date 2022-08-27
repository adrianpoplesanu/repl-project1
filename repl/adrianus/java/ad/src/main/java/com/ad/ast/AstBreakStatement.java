package com.ad.ast;

import com.ad.token.Token;

public class AstBreakStatement extends AstNode {

    private Token token;

    public AstBreakStatement(Token token) {
        type = AstNodeTypeEnum.BREAK_STATEMENT;
        this.token = token;
    }

    @Override
    public String tokenLiteral() {
        return null;
    }

    @Override
    public String toString() {
        return null;
    }
}
