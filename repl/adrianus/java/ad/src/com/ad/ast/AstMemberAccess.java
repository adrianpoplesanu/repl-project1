package com.ad.ast;

import com.ad.token.Token;

public class AstMemberAccess extends AstNode {
    private Token token;

    public AstMemberAccess() {
        type = AstNodeTypeEnum.MEMBER_ACCESS;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "AstMemeberAccess";
    }

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }
}
