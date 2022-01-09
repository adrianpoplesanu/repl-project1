package com.ad.ast;

import com.ad.token.Token;

public class AstPrefixIncrement extends AstNode {
    private Token token;

    public AstPrefixIncrement() {
        type = AstNodeTypeEnum.PREFIX_INCREMENT;
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
}
