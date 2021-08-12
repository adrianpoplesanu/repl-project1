import com.ad.ast.AstProgram;
import com.ad.parser.Parser;
import com.ad.repl.Repl;

public class Ad {

	public static void main(String[] args) {
		System.out.println("Ad intepreter [JAVA]... v1.0");
		//String source = "let a = 1; let b=45; if(a==b) { 10; }";
		//String source = "101; 123 456 67; true; false; 123 true 45 false 67;";

		Parser parser = new Parser();
		//parser.load(source);
		AstProgram program = new AstProgram();
		//parser.buildProgramStatements(program);
		
		Repl repl = new Repl(parser, program);
		repl.loop();
		//System.out.println(repl.getNextLine());
	}
}
