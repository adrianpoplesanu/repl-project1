package com.ad.lexer;

import com.ad.token.Token;
import com.ad.token.TokenKeywordConverter;
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
	
	public Lexer(String source) {
		this.source = source;
		currentChar = 0;
		position = 0;
		readPosition = 0;
		readChar();
	}
	
	public void load(String source) {
		this.source = source;
		currentChar = 0;
		position = 0;
		readPosition = 0;
		readChar();
	}
	
	public Token nextToken() {
		Token token = new Token();
		skipWhitespaces();
		boolean needs_next_char = true;
		switch(currentChar) {
		case 0:
			token.setType(TokenTypeEnum.EOF);
			token.setLiteral("");
		break;
		case '+':
			if (peekChar() == '+') {
				token.setType(TokenTypeEnum.PLUSPLUS);
				token.setLiteral("++");
				readChar();
			} else if (peekChar() == '=') {
				token.setType(TokenTypeEnum.PLUS_EQ);
				token.setLiteral("+=");
				readChar();
			} else {
				token.setType(TokenTypeEnum.PLUS);
				token.setLiteral("+");
			}
		break;
		case '-':
			if (peekChar() == '-') {
				token.setType(TokenTypeEnum.MINUSMINUS);
				token.setLiteral("--");
				readChar();
			} else if (peekChar() == '=') {
				token.setType(TokenTypeEnum.MINUS_EQ);
				token.setLiteral("-=");
				readChar();
			} else {
				token.setType(TokenTypeEnum.MINUS);
				token.setLiteral("-");
			}
		break;
		case '*':
			token.setType(TokenTypeEnum.ASTERISK);
			token.setLiteral("*");
		break;
		case '/':
			if (peekChar() == '*') {
				token.setType(TokenTypeEnum.MULTI_COMMENT);
				token.setLiteral("/*");
				readChar();
				readChar();
				while (!(currentChar == '*' && peekChar() == '/')) {
					readChar();
				}
				readChar();
				readChar();
			} else if (peekChar() == '/') {
				token.setType(TokenTypeEnum.SINGLE_COMMENT);
				token.setLiteral("//");
				readChar();
				while (currentChar != '\n' && currentChar != 0) {
					readChar();
				}
			} else {
				token.setType(TokenTypeEnum.SLASH);
				token.setLiteral("/");
			}
		break;
		case ';':
			token.setType(TokenTypeEnum.SEMICOLON);
			token.setLiteral(";");
		break;
		case ',':
			token.setType(TokenTypeEnum.COMMA);
			token.setLiteral(",");
		break;
		case '(':
			token.setType(TokenTypeEnum.LPAREN);
			token.setLiteral("(");
		break;
		case ')':
			token.setType(TokenTypeEnum.RPAREN);
			token.setLiteral(")");
		break;
		case '{':
			token.setType(TokenTypeEnum.LBRACE);
			token.setLiteral("{");
		break;
		case '}':
			token.setType(TokenTypeEnum.RBRACE);
			token.setLiteral("}");
		break;
		case '[':
			token.setType(TokenTypeEnum.LBRACKET);
			token.setLiteral("[");
		break;
		case ']':
			token.setType(TokenTypeEnum.RBRACKET);
			token.setLiteral("]");
		break;
		case '"':
			token.setType(TokenTypeEnum.DOUBLE_QUOTES);
			token.setLiteral(readDoubleQuotesString());
			break;
		case '\'':
			token.setType(TokenTypeEnum.SINGLE_QUOTES);
			token.setLiteral(readSingleQuotesString());
			break;
		case ':':
			token.setType(TokenTypeEnum.COLON);
			token.setLiteral(":");
			break;
		case '.':
			token.setType(TokenTypeEnum.DOT);
			token.setLiteral(".");
			break;
		case '<':
			if (peekChar() == '=') {
				token.setType(TokenTypeEnum.LTE);
				token.setLiteral("<=");
				readChar();
			} else {
				token.setType(TokenTypeEnum.LT);
				token.setLiteral("<");
			}
		break;
		case '>':
			if (peekChar() == '=') {
				token.setType(TokenTypeEnum.GTE);
				token.setLiteral(">=");
				readChar();
			} else {
				token.setType(TokenTypeEnum.GT);
				token.setLiteral(">");
			}
		break;
		case '=':
			if (peekChar() == '=') {
				token.setType(TokenTypeEnum.EQ);
				token.setLiteral("==");
				readChar();
			} else {
				token.setType(TokenTypeEnum.ASSIGN);
				token.setLiteral("=");
			}
		break;
		case '!':
			if (peekChar() == '=') {
				token.setType(TokenTypeEnum.NOT_EQ);
				token.setLiteral("!=");
				readChar();
			} else {
				token.setType(TokenTypeEnum.BANG);
				token.setLiteral("!");
			}
		break;
		default:
			if (isLetter()) {
				String literal = readIdentifier();
				token.setType(lookupKeyword(literal));
				token.setLiteral(literal);
				needs_next_char = false;
			} else
			if (isDigit()) {
				String literal = readNumber();
				if (literal.contains(".")) token.setType(TokenTypeEnum.FLOAT);
				else token.setType(TokenTypeEnum.INT);
				token.setLiteral(literal);
				needs_next_char = false;
			} else {
				token.setType(TokenTypeEnum.ILLEGAL);
				token.setLiteral(String.valueOf(currentChar));
			}
		break;
		}
		if (needs_next_char) readChar();
		return token;	
	}
	
	private char peekChar() {
		if (readPosition >= source.length()) return 0;
		return source.charAt(readPosition);
	}
	
	private boolean isDigit() {
		return ('0' <= currentChar) && (currentChar <= '9');
	}

	private boolean isFloatDot() {
		return currentChar == '.';
	}
	
	private boolean isLetter() {
		return (('a' <= currentChar) && (currentChar <= 'z')) || (('A' <= currentChar) && (currentChar <= 'Z')) || currentChar == '_';
	}
	
	private String readIdentifier() {
		int start = position;
		while(isLetter()) {
			readChar();
			while(isLetter() || isDigit()) {
				readChar();
			}
		}
		return source.substring(start, position);
	}
	
	private String readNumber() {
		int start = position;
		while(isDigit()) {
			readChar();
		}
		if (isFloatDot()) {
			readChar();
			while(isDigit()) {
				readChar();
			}
		}
		return source.substring(start, position);
	}
	
	private String readDoubleQuotesString() {
		readChar();
		int start = position;
		while(currentChar != '"') {
			if (currentChar == '\\' && peekChar() == '"') { // escaping \"
				readChar();
			}
			readChar();
		}
		return source.substring(start, position);
	}

	private String readSingleQuotesString() {
		readChar();
		int start = position;
		while(currentChar != '\'') {
			if (currentChar == '\\' && peekChar() == '\'') { // escaping \'
				readChar();
			}
			readChar();
		}
		return source.substring(start, position);
	}
	
	private TokenTypeEnum lookupKeyword(String literal) {
		if (TokenKeywordConverter.keywords.containsKey(literal)) return TokenKeywordConverter.convertToKeyword(literal);
		return TokenTypeEnum.IDENT;
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
