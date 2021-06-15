package com.ad.lexer;

import com.ad.token.Token;
import com.ad.token.TokenTypeEnum;

public class Lexer {
	private String source;
	private char currentChar;
	private int position;
	private int readPosition;

	public Lexer() {
		source = "";
		currentChar = 0;
		position = 0;
		readPosition = 0;
	}
	
	public Lexer(String s) {
		source = s;
		currentChar = 0;
		position = 0;
		readPosition = 0;
		readChar();
	}
	
	public void reset(String s) {
		source = s;
		currentChar = 0;
		position = 0;
		readPosition = 0;
		readChar();
	}
	
	public Token nextToken() {
		Token token = new Token();
		skipWhitespaces();
		switch(currentChar) {
		case 0:
			token.setType(TokenTypeEnum.EOF);
			token.setLiteral("");
		break;
		case '+':
			token.setType(TokenTypeEnum.PLUS);
			token.setLiteral("+");
		break;
		case '-':
			token.setType(TokenTypeEnum.MINUS);
			token.setLiteral("-");
		break;
		case '*':
			token.setType(TokenTypeEnum.ASTERISK);
			token.setLiteral("*");
		break;
		case '/':
			token.setType(TokenTypeEnum.SLASH);
			token.setLiteral("/");
		break;
		case ';':
			token.setType(TokenTypeEnum.SEMICOLON);
			token.setLiteral(";");
		break;
		case ',':
			token.setType(TokenTypeEnum.COMMA);
			token.setLiteral(",");
		break;
		default:
			if (isLetter()) {
				String literal = readIdentifier();
				token.setType(TokenTypeEnum.IDENT);
				// token.setType(lookupKeyword(literal));
				token.setLiteral(literal);
			} else
			if (isDigit()) {
				String literal = readNumber();
				token.setType(TokenTypeEnum.INT);
				token.setLiteral(literal);
			} else {
				token.setType(TokenTypeEnum.ILLEGAL);
				token.setLiteral("");
			}
		break;
		}
		readChar();
		return token;	
	}
	
	private boolean isDigit() {
		return ('0' <= currentChar) && (currentChar <= '9');
	}
	
	private boolean isLetter() {
		return (('a' <= currentChar) && (currentChar <= 'z')) || (('A' <= currentChar) && (currentChar <= 'Z')) || currentChar == '_';
	}
	
	private String readIdentifier() {
		int start = position;
		while(isLetter()) {
			readChar();
		}
		return source.substring(start, position);
	}
	
	private String readNumber() {
		int start = position;
		while(isDigit()) {
			readChar();
		}
		return source.substring(start, position);
	}
	
	private TokenTypeEnum lookupKeyword(String literal) {
		return TokenTypeEnum.ILLEGAL;
	}
	
	private void skipWhitespaces() {
		while((currentChar == ' ') || (currentChar == '\t') || (currentChar == '\r') || (currentChar == '\n')) {
			readChar();
		}
	}
	
	private void readChar() {
		if (readPosition >= source.length()) {
			currentChar = 0;
		} else {
			currentChar = source.charAt(readPosition);
		}
		position = readPosition;
		readPosition++;
	}
}
