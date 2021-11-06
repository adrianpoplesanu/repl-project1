package com.ad.ast;

import com.ad.token.Token;

public class AstAssignStatement extends AstNode {
    private Token token;

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
}
