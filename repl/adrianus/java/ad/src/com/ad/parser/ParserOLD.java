package com.ad.parser;

import java.util.ArrayList;
import java.util.HashMap;

import com.ad.ast.*;
import com.ad.lexer.Lexer;
import com.ad.token.Token;
import com.ad.token.TokenTypeEnum;

public class ParserOLD {
	private Lexer lexer;
	private String source;
	private Token currentToken;
	private Token peekToken;
	private HashMap<TokenTypeEnum, PrefixParseInterface> prefixParseFns = new HashMap<TokenTypeEnum, PrefixParseInterface>();
	private HashMap<TokenTypeEnum, InfixParseInterface> infixParseFns = new HashMap<TokenTypeEnum, InfixParseInterface>();
	
	public ParserOLD() {
		//... i should instantiate the callback maps here for prefix and infix
		lexer = new Lexer();
		prefixParseFns.put(TokenTypeEnum.IDENT, new IdentifierParser(this));
		prefixParseFns.put(TokenTypeEnum.INT, new IntegerLiteralParser(this));
		prefixParseFns.put(TokenTypeEnum.BANG, new PrefixExpressionParser(this));
		prefixParseFns.put(TokenTypeEnum.MINUS, new PrefixExpressionParser(this));
		prefixParseFns.put(TokenTypeEnum.TRUE, new BooleanParser(this));
		prefixParseFns.put(TokenTypeEnum.FALSE, new BooleanParser(this));
		prefixParseFns.put(TokenTypeEnum.LPAREN, new GroupExpressionParser(this));
	    prefixParseFns.put(TokenTypeEnum.IF, new IfExpressionParser(this));
	    prefixParseFns.put(TokenTypeEnum.FUNCTION, new FunctionLiteralParser(this));
	    prefixParseFns.put(TokenTypeEnum.WHILE, new WhileExpressionParser(this));
	    prefixParseFns.put(TokenTypeEnum.FOR, new ForExpressionParser(this));
	    prefixParseFns.put(TokenTypeEnum.DEF, new DefStatementParser(this));
	    prefixParseFns.put(TokenTypeEnum.LBRACKET, new ListLiteralParser(this));
		prefixParseFns.put(TokenTypeEnum.LBRACE, new HashLiteralParser(this));
	    prefixParseFns.put(TokenTypeEnum.STRING, new StringLiteralParser(this));
		infixParseFns.put(TokenTypeEnum.PLUS, new InfixExpressionParser(this));
		infixParseFns.put(TokenTypeEnum.MINUS, new InfixExpressionParser(this));
		infixParseFns.put(TokenTypeEnum.ASTERISK, new InfixExpressionParser(this));
		infixParseFns.put(TokenTypeEnum.SLASH, new InfixExpressionParser(this));
		infixParseFns.put(TokenTypeEnum.EQ, new InfixExpressionParser(this));
		infixParseFns.put(TokenTypeEnum.NOT_EQ, new InfixExpressionParser(this));
		infixParseFns.put(TokenTypeEnum.LT, new InfixExpressionParser(this));
		infixParseFns.put(TokenTypeEnum.GT, new InfixExpressionParser(this));
		infixParseFns.put(TokenTypeEnum.LTE, new InfixExpressionParser(this));
		infixParseFns.put(TokenTypeEnum.GTE, new InfixExpressionParser(this));
		infixParseFns.put(TokenTypeEnum.LPAREN, new CallExpressionParser(this));
		infixParseFns.put(TokenTypeEnum.LBRACKET, new IndexExpressionParser(this));
		infixParseFns.put(TokenTypeEnum.ASSIGN, new AssignExpressionParser(this));
	}

	public void load(String source) {
		this.source = source;
		lexer.load(this.source);
		nextToken();
		nextToken();
	}
	
	public void buildProgramStatements(AstProgram program) {
		while (currentToken.getType() != TokenTypeEnum.EOF) {
			AstNode node = parseStatement();
			if (node != null) {
				program.statements.add(node);
			}
			nextToken();
		}
	}
	
	public void nextToken() {
		currentToken = peekToken;
		peekToken = lexer.nextToken();
	}
	
	public boolean currentTokenIs(TokenTypeEnum tte) {
		return currentToken.getType() == tte;
	}

	public boolean peekTokenIs(TokenTypeEnum tte) {
		return peekToken.getType() == tte;
	}

	public boolean expectPeek(TokenTypeEnum tte) {
		if (peekTokenIs(tte)) {
			nextToken();
			return true;
		} else {
			return false;
		}
	}
	
	private PrecedenceTypeEnum currentPrecedence() {
		if (TokenPrecedenceConverter.precedenceMap.containsKey(currentToken.getType())) {
			return TokenPrecedenceConverter.precedenceMap.get(currentToken.getType());
		}
		return PrecedenceTypeEnum.LOWEST;
	}

	private PrecedenceTypeEnum peekPrecedence() {
		if (TokenPrecedenceConverter.precedenceMap.containsKey(peekToken.getType())) {
			return TokenPrecedenceConverter.precedenceMap.get(peekToken.getType());
		}
		return PrecedenceTypeEnum.LOWEST;
	}

	public AstNode parseStatement() {
		if (currentToken.getType() == TokenTypeEnum.LET) {
			return parseLetStatement();
		} else if (currentToken.getType() == TokenTypeEnum.RETURN) {
			return parseReturnStatement();
		} else {
			return parseExpressionStatement();
		}
	}

	public AstNode parseLetStatement() {
		AstLetStatement stmt = new AstLetStatement(currentToken);
		if (!expectPeek(TokenTypeEnum.IDENT)) {
			return null;
		}
		stmt.setName(new AstIdentifier(currentToken, currentToken.getLiteral()));
		if (!expectPeek(TokenTypeEnum.ASSIGN)) {
			return null;
		}
		nextToken();
		stmt.setValue(parseExpression(PrecedenceTypeEnum.LOWEST));
		if (currentTokenIs(TokenTypeEnum.SEMICOLON)) {
			nextToken();
		}
		return stmt;
	}

	public AstNode parseReturnStatement() {
		AstReturnStatement stmt = new AstReturnStatement(currentToken);
		nextToken();
		stmt.setValue(parseExpression(PrecedenceTypeEnum.LOWEST));
		if (peekTokenIs(TokenTypeEnum.SEMICOLON) || peekTokenIs(TokenTypeEnum.RBRACE) || peekTokenIs(TokenTypeEnum.EOF)) {
			return stmt;
		}
		while(!currentTokenIs(TokenTypeEnum.SEMICOLON) && !currentTokenIs(TokenTypeEnum.RBRACE) && !currentTokenIs(TokenTypeEnum.EOF)) {
			nextToken();
		}
		return stmt;
	}

	public AstNode parseExpressionStatement() {
		AstExpressionStatement stmt = new AstExpressionStatement(currentToken);
		stmt.setExpression(parseExpression(PrecedenceTypeEnum.LOWEST));
		if (peekTokenIs(TokenTypeEnum.SEMICOLON)) {
			nextToken();
		}
		return stmt;
	}

    // TODO: PrefixParseInterface and InfixParseInterface should call specific public methods from within the parser
    // it makes more sense to have the logic that is responsible with parsing here and not in the interfaces

	public AstNode parseIdentifier() {
		return new AstIdentifier(getCurrentToken(), getCurrentToken().getLiteral());
	}

	public AstNode parseInteger() {
		String value = getCurrentToken().getLiteral();
		return new AstInteger(getCurrentToken(), Integer.parseInt(value));
	}

	public AstNode parsePrefixExpression() {
		AstPrefixExpression expr = new AstPrefixExpression(getCurrentToken(),
				getCurrentToken().getLiteral());
		nextToken();
		expr.setRight(parseExpression(PrecedenceTypeEnum.PREFIX));
		return expr;
	}

	public AstNode parseBoolean() {
		return new AstBoolean(getCurrentToken(), currentTokenIs(TokenTypeEnum.TRUE));
	}

	public AstNode parseGroupedExpression() {
		nextToken();
		AstNode expr = parseExpression(PrecedenceTypeEnum.LOWEST);
		if (!expectPeek(TokenTypeEnum.RPAREN)) {
			return null;
		}
		return expr;
	}

	public AstNode parseIfExpression() {
		AstIfExpression expr = new AstIfExpression(getCurrentToken());
		if (!expectPeek(TokenTypeEnum.LPAREN)) {
			return null;
		}
		nextToken();
		expr.setCondition(parseExpression(PrecedenceTypeEnum.LOWEST));
		if (!expectPeek(TokenTypeEnum.RPAREN)) {
			return null;
		}
		if (!expectPeek(TokenTypeEnum.LBRACE)) {
			return null;
		}
		expr.setConsequence(parseBlockStatement());
		if (peekTokenIs(TokenTypeEnum.ELSE)) {
			nextToken();
			if (!expectPeek(TokenTypeEnum.LBRACE)) {
				return null;
			}
			expr.setAlternative(parseBlockStatement());
		}
		return expr;
	}

	private AstNode parseBlockStatement() {
		AstBlockStatement block = new AstBlockStatement(getCurrentToken());
		nextToken();
		while(!currentTokenIs(TokenTypeEnum.RBRACE) && !currentTokenIs(TokenTypeEnum.EOF)) {
			AstNode statement = parseStatement();
			if (statement != null) {
				block.addStatement(statement);
			}
			nextToken();
		}
		return block;
	}
	
	public AstNode parseFunctionLiteral() {
		AstFunctionLiteral func = new AstFunctionLiteral(getCurrentToken());
		if (!expectPeek(TokenTypeEnum.LPAREN)) {
			return null;
		}
		func.setParameters(parseFunctionParameters());
		if (!expectPeek(TokenTypeEnum.LBRACE)) {
			return null;
		}
		func.setBody(parseBlockStatement());
		return func;
	}
	
	public ArrayList<AstNode> parseFunctionParameters() {
		ArrayList<AstNode> identifiers = new ArrayList<AstNode>();
		if (peekTokenIs(TokenTypeEnum.RPAREN)) {
			nextToken();
			return identifiers;
		}
		nextToken();
		AstIdentifier ident = new AstIdentifier(getCurrentToken(), getCurrentToken().getLiteral());
		identifiers.add(ident);
		while(peekTokenIs(TokenTypeEnum.COMMA)) {
			nextToken();
			nextToken();
			ident = new AstIdentifier(getCurrentToken(), getCurrentToken().getLiteral());
			identifiers.add(ident);
		}
		if (!expectPeek(TokenTypeEnum.RPAREN)) {
			return new ArrayList<AstNode>(); // return an empty list
		}
		return identifiers;
	}
	
	public AstNode parseCallExpression(AstNode node) {
		AstCallExpression expr = new AstCallExpression(getCurrentToken(), node);
		expr.setArguments(parseCallArguments());
		return expr;
	}
	
	public ArrayList<AstNode> parseCallArguments() {
		ArrayList<AstNode> args = new ArrayList<AstNode>();
		if (peekTokenIs(TokenTypeEnum.RPAREN)) {
			nextToken();
			return args;
		}
		nextToken();
		args.add(parseExpression(PrecedenceTypeEnum.LOWEST));
		while(peekTokenIs(TokenTypeEnum.COMMA)) {
			nextToken();
			nextToken();
			args.add(parseExpression(PrecedenceTypeEnum.LOWEST));
		}
		if (!expectPeek(TokenTypeEnum.RPAREN)) {
			return new ArrayList<AstNode>();
		}
		return args;
	}

	public AstNode parseInfixExpression(AstNode left) {
		AstInfixExpression expr = new AstInfixExpression(getCurrentToken(), getCurrentToken().getLiteral(), left);
		PrecedenceTypeEnum precedence = currentPrecedence();
		nextToken();
		expr.setRight(parseExpression(precedence));
		return expr;
	}

	public AstNode parseListLiteral() {
		return null;
	}

	public AstNode parseHashLiteral() {
		return null;
	}

	public AstNode parseIndexExpression(AstNode node) {
		return null;
	}

	public AstNode parseAssignExpression(AstNode node) {
		AstAssignStatement stmt = new AstAssignStatement(currentToken);
		return null;
	}

	public AstNode parseExpression(PrecedenceTypeEnum pte) {
		if (!prefixParseFns.containsKey(currentToken.getType())) {
			return null;
		}
		PrefixParseInterface prefixParser = prefixParseFns.get(currentToken.getType());
		AstNode left_expression = prefixParser.parse();
		while(!peekTokenIs(TokenTypeEnum.SEMICOLON) && (pte.ordinal() < peekPrecedence().ordinal())) {
			if (!infixParseFns.containsKey(peekToken.getType())) return left_expression;
			InfixParseInterface infixParser = infixParseFns.get(peekToken.getType());
			nextToken();
			left_expression = infixParser.parse(left_expression);
		}
		return left_expression;
	}
	
	public AstNode parseStringLiteral() {
		AstStringLiteral node = new AstStringLiteral(getCurrentToken());
		node.setValue(getCurrentToken().getLiteral());
		return node;
	}
	
	public AstNode parseWhileExpression() {
		AstWhileExpression expr = new AstWhileExpression(getCurrentToken());
		if (!expectPeek(TokenTypeEnum.LPAREN)) {
			return null; // this should all return error ast nodes that evaluate in error objects
		}
		nextToken();
		expr.setCondition(parseExpression(PrecedenceTypeEnum.LOWEST));
		if (!expectPeek(TokenTypeEnum.RPAREN)) {
			return null; // see previous comment
		}
		if (!expectPeek(TokenTypeEnum.LBRACE)) {
			return null; // see previous comment
		}
		expr.setBody(parseBlockStatement());
		return expr;
	}

	public AstNode parseForExpression() {
		// TODO: implement this
		return null;
	}

	public AstNode parseDefStatement() {
		// TODO: implement this
		return null;
	}

	public Lexer getLexer() {
		return lexer;
	}

	public void setLexer(Lexer lexer) {
		this.lexer = lexer;
	}

	public String getSource() {
		return source;
	}

	public void setSource(String source) {
		this.source = source;
	}

	public Token getCurrentToken() {
		return currentToken;
	}

	public void setCurrentToken(Token currentToken) {
		this.currentToken = currentToken;
	}

	public Token getPeekToken() {
		return peekToken;
	}

	public void setPeekToken(Token peekToken) {
		this.peekToken = peekToken;
	}

	public HashMap<TokenTypeEnum, PrefixParseInterface> getPrefixParseFns() {
		return prefixParseFns;
	}

	public void setPrefixParseFns(HashMap<TokenTypeEnum, PrefixParseInterface> prefixParseFns) {
		this.prefixParseFns = prefixParseFns;
	}

	public HashMap<TokenTypeEnum, InfixParseInterface> getInfixParseFns() {
		return infixParseFns;
	}

	public void setInfixParseFns(HashMap<TokenTypeEnum, InfixParseInterface> infixParseFns) {
		this.infixParseFns = infixParseFns;
	}
}