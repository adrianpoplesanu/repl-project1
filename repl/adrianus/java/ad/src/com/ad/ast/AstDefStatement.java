package com.ad.ast;

import com.ad.token.Token;

import java.util.ArrayList;

public class AstDefStatement extends AstNode {
    private Token token;
    private AstNode name;
    private ArrayList<AstNode> parameters;
    private AstNode body;

    public AstDefStatement() {
        type = AstNodeTypeEnum.DEF_STATEMENT;
    }

    public AstDefStatement(Token t) {
        this();
        token = t;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "AstDefStatement";
    }

    public Token getToken() {
        return token;
    }

    public void setToken(Token token) {
        this.token = token;
    }

    public AstNode getName() {
        return name;
    }

    public void setName(AstNode name) {
        this.name = name;
    }

    public ArrayList<AstNode> getParameters() {
        return parameters;
    }

    public void setParameters(ArrayList<AstNode> parameters) {
        this.parameters = parameters;
    }

    public AstNode getBody() {
        return body;
    }

    public void setBody(AstNode body) {
        this.body = body;
    }
}
