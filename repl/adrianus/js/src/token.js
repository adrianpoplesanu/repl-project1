function Token(tokenType, literal) {
	this.tokenType = tokenType;
	this.literal = literal;
}

var token_type = {
	ILLEGAL: 'ILLEGAL',
	IDENT: 'IDENT',
	RETURN: 'RETURN',
	TRUE: 'TRUE',
	FALSE: 'FALSE'
};