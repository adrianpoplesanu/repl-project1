package com.ad.ast;

import com.ad.token.Token;

import java.util.List;

public class AstListExpression extends AstNode {
    private Token token;
    private List<AstNode> elements;

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

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }

    public List<AstNode> getElements() {
        return elements;
    }

    public void setElements(List<AstNode> elements) {
        this.elements = elements;
    }
}
