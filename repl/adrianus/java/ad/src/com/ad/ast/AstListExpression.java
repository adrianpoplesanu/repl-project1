package com.ad.ast;

import com.ad.token.Token;

public class AstListExpression extends AstNode {
    private Token token;

    public AstListExpression(Token token) {
        type = AstNodeTypeEnum.LIST_EXPRESSION;
        this.token = token;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "AstListExpression";
    }
}
