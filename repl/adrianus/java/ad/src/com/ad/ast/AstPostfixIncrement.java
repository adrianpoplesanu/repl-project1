package com.ad.ast;

import com.ad.token.Token;

public class AstPostfixIncrement extends AstNode {
    private Token token;

    public AstPostfixIncrement() {
        type = AstNodeTypeEnum.POSTFIX_INCREMENT;
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
}
