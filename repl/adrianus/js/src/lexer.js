function Lexer() {
	this.source = '';
	this.position = 0;
	this.readPosition = 0;
	this.current_char = '';
}

Lexer.prototype.load = function(source) {
	this.source = source;
	this.position = 0;
	this.readPosition = 0;
	this.current_char = '';
}

Lexer.prototype.readChar = function () {
	this.current_char = this.source[this.readPosition];
	this.position = this.readPosition;
	this.readPosition++;
}

Lexer.prototype.stillHasChars = function () {
	console.log(this.source.length);
	return this.position < this.source.length;
}