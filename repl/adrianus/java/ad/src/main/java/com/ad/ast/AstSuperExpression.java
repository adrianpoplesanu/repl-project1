package com.ad.ast;

import com.ad.token.Token;

public class AstSuperExpression extends AstNode {

    private Token token;
    private AstNode target;

    public AstSuperExpression() {
        type = AstNodeTypeEnum.SUPER_EXPRESSION;
    }

    public AstSuperExpression(Token token) {
        this();
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

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }

    public AstNode getTarget() {
        return target;
    }

    public void setTarget(AstNode target) {
        this.target = target;
    }
}
