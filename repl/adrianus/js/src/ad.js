// this is the entry point for the NodeJS Ad language interpreter
console.log("Ad interpreter [Javascript]... v1.0");

eval(fs.readFileSync('src/ast.js')+'');
eval(fs.readFileSync('src/lexer.js')+'');
eval(fs.readFileSync('src/objects.js')+'');
eval(fs.readFileSync('src/repl.js')+'');
eval(fs.readFileSync('src/token.js')+'');