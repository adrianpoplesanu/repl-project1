#include "lexer.h"

Lexer::Lexer() {

}

Lexer::~Lexer() {

}

void Lexer::Load(std::string s) {
	source = s;
}

void Lexer::ReadChar() {
	if (readPosition < source.length()) {
		current_char = source[readPosition];
	} else {
		current_char = 0;
	}
	position = readPosition;
	++readPosition;
}

void Lexer::SkipWhitespace() {
	if (current_char == ' ' || current_char == '\t' || current_char == '\r' || current_char == '\n') {
		ReadChar();
	}
}

bool Lexer::IsLetter() {
	return (('a' <= current_char) && (current_char <= 'z')) || (('A' <= current_char) && (current_char <= 'Z')) || current_char == '_';
}

bool Lexer::IsDigit() {
	return ('0' <= current_char) && (current_char <= '9');
}

std::string Lexer::ReadIdentifier() {
	// TODO
	return "";
}

std::string Lexer::ReadNumber() {
	// TODO
	return "";
}

TokenType Lexer::LookupIdent(std::string ident) {
	// TODO
	return TT_ILLEGAL;
}

char Lexer::PeekChar() {
	// TODO
	return 0;
}

Token Lexer::NextToken() {
	// TODO
	Token t("", TT_ILLEGAL);
	return t;
}