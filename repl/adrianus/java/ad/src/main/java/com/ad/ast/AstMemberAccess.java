package com.ad.ast;

import com.ad.token.Token;

import java.util.ArrayList;
import java.util.List;

public class AstMemberAccess extends AstNode {
    private Token token;
    private AstNode owner;
    private AstNode member;
    private List<AstNode> arguments;
    private boolean isMethod;

    public AstMemberAccess() {
        type = AstNodeTypeEnum.MEMBER_ACCESS;
    }

    public AstMemberAccess(Token token) {
        this();
        this.token = token;
        arguments = new ArrayList<>();
    }

    public void clear() {
        arguments.clear();
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

    public AstNode getOwner() {
        return owner;
    }

    public void setOwner(AstNode owner) {
        this.owner = owner;
    }

    public AstNode getMember() {
        return member;
    }

    public void setMember(AstNode member) {
        this.member = member;
    }

    public List<AstNode> getArguments() {
        return arguments;
    }

    public void setArguments(List<AstNode> arguments) {
        this.arguments = arguments;
    }

    public boolean isMethod() {
        return isMethod;
    }

    public void setIsMethod(boolean method) {
        isMethod = method;
    }
}
