console.log("loaded repl.js");

function Repl(lexer) {
	this.source = '';
	this.lexer = lexer;
}

Repl.prototype.load = function(source) {
	this.source = source;
	this.lexer.load(source);
	this.lexer.readChar();
}

Repl.prototype.run = function () {
	//console.log(this.source);
	while (this.lexer.stillHasChars()) {
		console.log(this.lexer.current_char);
		this.lexer.readChar();
	}
}