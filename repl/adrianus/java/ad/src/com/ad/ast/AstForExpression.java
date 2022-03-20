package com.ad.ast;

import com.ad.token.Token;

public class AstForExpression extends AstNode {
    private Token token;
    private AstNode initialization;
    private AstNode condition;
    private AstNode step;
    private AstNode body;

    public AstForExpression() {
        type = AstNodeTypeEnum.FOR_EXPRESSION;
    }

    public AstForExpression(Token token) {
        this();
        this.token = token;
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

    public AstNode getInitialization() {
        return initialization;
    }

    public void setInitialization(AstNode initialization) {
        this.initialization = initialization;
    }

    public AstNode getCondition() {
        return condition;
    }

    public void setCondition(AstNode condition) {
        this.condition = condition;
    }

    public AstNode getStep() {
        return step;
    }

    public void setStep(AstNode step) {
        this.step = step;
    }

    public AstNode getBody() {
        return body;
    }

    public void setBody(AstNode body) {
        this.body = body;
    }
}
