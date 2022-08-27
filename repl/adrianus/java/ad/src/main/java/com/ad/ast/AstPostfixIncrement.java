package com.ad.ast;

import com.ad.token.Token;

public class AstPostfixIncrement extends AstNode {
    private Token token;
    private AstNode name;
    private String operation;

    public AstPostfixIncrement() {
        type = AstNodeTypeEnum.POSTFIX_INCREMENT;
    }

    public AstPostfixIncrement(Token token) {
        this();
        this.token = token;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "AstPostfixIncrement";
    }

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }

    public AstNode getName() {
        return name;
    }

    public void setName(AstNode name) {
        this.name = name;
    }

    public String getOperation() {
        return operation;
    }

    public void setOperation(String operation) {
        this.operation = operation;
    }
}
