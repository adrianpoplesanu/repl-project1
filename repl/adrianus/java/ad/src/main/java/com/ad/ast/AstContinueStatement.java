package com.ad.ast;

import com.ad.token.Token;

public class AstContinueStatement extends AstNode {
    private Token token;

    public AstContinueStatement(Token token) {
        type = AstNodeTypeEnum.CONTINUE_STATEMENT;
        this.token = token;
    }

    @Override
    public String tokenLiteral() {
        return null;
    }

    @Override
    public Token getToken() {
        return token;
    }

    @Override
    public String toString() {
        return null;
    }
}
