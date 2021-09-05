import com.ad.ast.AstProgram;
import com.ad.evaluator.Evaluator;
import com.ad.parser.Parser;
import com.ad.parser.test.ParserTest;
import com.ad.repl.Repl;

public class Ad {

	public static void main(String[] args) {
		System.out.println("Ad intepreter [JAVA]... v1.0");

        ParserTest.testParser();

		Parser parser = new Parser();
		AstProgram program = new AstProgram();
		Evaluator evaluator = new Evaluator();

		Repl repl = new Repl(parser, program, evaluator);
		repl.loop();
	}
}
