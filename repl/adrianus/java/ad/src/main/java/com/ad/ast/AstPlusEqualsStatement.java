package com.ad.ast;

import com.ad.token.Token;

import static com.ad.ast.AstNodeTypeEnum.PLUS_EQUALS_STATEMENT;

public class AstPlusEqualsStatement extends AstNode {
    private Token token;
    public AstNode name;
    public AstNode value;

    public AstPlusEqualsStatement() {
        type = PLUS_EQUALS_STATEMENT;
    }

    public AstPlusEqualsStatement(Token t) {
        this();
        token = t;
    }

    @Override
    public String tokenLiteral() {
        return token.getLiteral();
    }

    @Override
    public Token getToken() {
        return token;
    }

    @Override
    public String toString() {
        return null;
    }
}
