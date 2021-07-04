import { Lexer } from "./lexer";
import { Parser } from "./parser";

class Repl {
    lexer: Lexer;
    parser: Parser;
    public constructor(lexer: Lexer, parser: Parser) {
        this.lexer = lexer;
        this.parser = parser;
    }

    public execute_file() {
        //...
    }

    public loop() {
        //...
    }
}