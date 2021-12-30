package com.ad.ast;

import com.ad.token.Token;

import java.util.HashMap;

public class AstHashExpression extends AstNode {
    private Token token;
    private HashMap<AstNode, AstNode> pairs = new HashMap<>();

    public AstHashExpression() {
        type = AstNodeTypeEnum.HASH_EXPRESSION;
    }

    public AstHashExpression(Token t) {
        this();
        token = t;
    }

    public void AddPair(AstNode key, AstNode value) {
        pairs.put(key, value);
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "AstHashExpression";
    }

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }

    public HashMap<AstNode, AstNode> getPairs() {
        return pairs;
    }

    public void setPairs(HashMap<AstNode, AstNode> pairs) {
        this.pairs = pairs;
    }
}
