package com.ad.ast;

import com.ad.token.Token;

import java.util.ArrayList;
import java.util.List;

public class AstClassStatement extends AstNode {
    private Token token;
    private AstNode name;
    private List<AstNode> methods;
    private List<AstNode> attributes;
    private List<AstNode> inheritFrom;

    public AstClassStatement() {
        type = AstNodeTypeEnum.CLASS_STATEMENT;
    }

    public AstClassStatement(Token t) {
        this();
        token = t;
        methods = new ArrayList<>();
        attributes = new ArrayList<>();
        inheritFrom = new ArrayList<>();
    }

    public AstClassStatement(Token t, AstNode n) {
        this(t);
        name = n;
    }

    public AstClassStatement(Token t, AstNode n, List<AstNode> m, List<AstNode> a) {
        this(t, n);
        methods = m;
        attributes = a;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public String toString() {
        return "AstClass";
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

    public List<AstNode> getMethods() {
        return methods;
    }

    public void setMethods(List<AstNode> methods) {
        this.methods = methods;
    }

    public List<AstNode> getAttributes() {
        return attributes;
    }

    public void setAttributes(List<AstNode> attributes) {
        this.attributes = attributes;
    }

    public List<AstNode> getInheritFrom() {
        return inheritFrom;
    }

    public void setInheritFrom(List<AstNode> inheritFrom) {
        this.inheritFrom = inheritFrom;
    }
}
