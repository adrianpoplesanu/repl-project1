package com.ad.ast;

import com.ad.token.Token;
import com.ad.token.TokenTypeEnum;

public class AstIndexExpression extends AstNode {
    private Token token;
    private AstNode left;
    private AstNode index;

    public AstIndexExpression() {
        type = AstNodeTypeEnum.INDEX_EXPRESSION;
    }

    public AstIndexExpression(Token t) {
        this();
        token = t;
    }

    public AstIndexExpression(Token t, AstNode l) {
        this(t);
        left = l;
    }

    @Override
    public String tokenLiteral() {
        return null;
    }

    @Override
    public String toString() {
        return null;
    }

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }

    public AstNode getLeft() {
        return left;
    }

    public void setLeft(AstNode left) {
        this.left = left;
    }

    public AstNode getIndex() {
        return index;
    }

    public void setIndex(AstNode index) {
        this.index = index;
    }
}
