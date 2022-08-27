package com.ad.ast;

import com.ad.token.Token;

public class AstAssignStatement extends AstNode {
    private Token token;
    private AstNode value;
    private AstNode name;

    public AstAssignStatement() {
        type = AstNodeTypeEnum.ASSIGN_STATEMENT;
    }

    public AstAssignStatement(Token token) {
        this();
        this.token = token;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "TODO: implement this in AstAssignStatement";
    }

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }

    public AstNode getValue() {
        return value;
    }

    public void setValue(AstNode value) {
        this.value = value;
    }

    public AstNode getName() {
        return name;
    }

    public void setName(AstNode name) {
        this.name = name;
    }
}
