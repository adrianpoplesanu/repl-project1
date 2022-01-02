package com.ad.ast;

import com.ad.token.Token;

public class AstThisExpression extends AstNode {
    private Token token;

    public AstThisExpression() {
        type = AstNodeTypeEnum.THIS_EXPRESSION;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "AstThisExpression";
    }

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }
}
