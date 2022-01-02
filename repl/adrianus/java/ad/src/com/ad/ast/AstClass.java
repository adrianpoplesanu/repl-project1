package com.ad.ast;

import com.ad.token.Token;

public class AstClass extends AstNode {
    private Token token;

    public AstClass() {
        type = AstNodeTypeEnum.CLASS_STATEMENT;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "AstClass";
    }

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }
}
