package com.ad.ast;

import com.ad.token.Token;

public class AstComment extends AstNode {
    private Token token;

    public AstComment() {
        type = AstNodeTypeEnum.COMMENT;
    }

    public AstComment(Token token) {
        this();
        this.token = token;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "AstComment";
    }

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }
}
