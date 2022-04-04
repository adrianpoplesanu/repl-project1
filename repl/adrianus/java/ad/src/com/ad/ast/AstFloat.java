package com.ad.ast;

import com.ad.token.Token;

public class AstFloat extends AstNode {
    private Token token;
    private float value;

    public AstFloat() {
        type = AstNodeTypeEnum.FLOAT_LITERAL;
    }

    public AstFloat(Token token) {
        this();
        this.token = token;
    }

    public AstFloat(Token token, float value) {
        this(token);
        this.value = value;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "FloatLiteral [" + token.getLiteral() + "] <" + value + ">";
    }

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }

    public float getValue() {
        return value;
    }

    public void setValue(float value) {
        this.value = value;
    }
}
