package com.ad.ast;

import com.ad.token.Token;

public class AstNullExpression extends AstNode {
    private Token token;

    public AstNullExpression() {
        type = AstNodeTypeEnum.NULL_EXPRESSION;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "AstNullExpression";
    }

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }
}
