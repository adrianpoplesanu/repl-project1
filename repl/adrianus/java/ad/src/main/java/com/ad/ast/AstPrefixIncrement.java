package com.ad.ast;

import com.ad.token.Token;

public class AstPrefixIncrement extends AstNode {
    private Token token;
    private AstNode name;
    private String operation;

    public AstPrefixIncrement() {
        type = AstNodeTypeEnum.PREFIX_INCREMENT;
    }

    public AstPrefixIncrement(Token token) {
        this();
        this.token = token;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "AstPrefixExpression";
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
