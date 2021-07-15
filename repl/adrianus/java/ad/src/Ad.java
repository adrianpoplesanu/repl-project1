import com.ad.ast.AstProgram;
import com.ad.parser.Parser;

public class Ad {

	public static void main(String[] args) {
		System.out.println("Ad intepreter [JAVA]... v1.0");
		String source = "let a = 1; let b=45; if(a==b) { 10; }";
		//lexer.load(source);
		//Token currentToken = lexer.nextToken();
		//while(currentToken.getType() != TokenTypeEnum.EOF) {
		//	System.out.println(currentToken);
		//	currentToken = lexer.nextToken();
		//}
		Parser parser = new Parser();
		parser.load(source);
		AstProgram program = new AstProgram();
		parser.buildProgramStatements(program);
	}
}
