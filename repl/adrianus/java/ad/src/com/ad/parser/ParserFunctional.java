package com.ad.parser;

import com.ad.ast.AstNode;
import com.ad.lexer.Lexer;
import com.ad.objects.AdObject;
import com.ad.token.Token;
import com.ad.token.TokenTypeEnum;

import java.util.HashMap;
import java.util.function.Function;

public class ParserFunctional {
    private Lexer lexer;
    private String source;
    private Token currentToken;
    private Token peekToken;
    private HashMap<TokenTypeEnum, Function<AstNode, AdObject>> prefixParseFns = new HashMap<>();
    private HashMap<TokenTypeEnum, Function<AstNode, AdObject>> infixParserFns = new HashMap<>();

    public ParserFunctional() {
        lexer = new Lexer();
        prefixParseFns.put(TokenTypeEnum.IDENT, this::parseIdentifier);

    }

    AdObject parseIdentifier(AstNode node) {
        return null;
    }

    AdObject parseExpression(AstNode node) {
        return null;
    }
}
