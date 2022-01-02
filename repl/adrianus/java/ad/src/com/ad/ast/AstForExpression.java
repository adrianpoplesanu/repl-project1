package com.ad.ast;

import com.ad.token.Token;

public class AstForExpression extends AstNode {
    private Token token;

    public AstForExpression() {
        type = AstNodeTypeEnum.FOR_EXPRESSION;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "AstForExpression";
    }

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }
}
