import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Paths;

import com.ad.ast.AstProgram;
import com.ad.evaluator.Evaluator;
import com.ad.parser.Parser;
import com.ad.parser.test.ParserTest;
import com.ad.repl.Repl;

public class Ad {

	public static void main(String[] args) {
		if (args.length > 0) {
			String filename = args[0];
			String source;
			try {
				source = new String(Files.readAllBytes(Paths.get(filename)));
			} catch (IOException e) {
				source = "";
			}
			//System.out.println(source);
			Parser parser = new Parser();
			AstProgram program = new AstProgram();
			Evaluator evaluator = new Evaluator();

			Repl repl = new Repl(parser, program, evaluator);
			repl.executeSource(source);
		} else {
			System.out.println("Ad interpreter [JAVA]... v1.2");
			
	        ParserTest.testParser();
	
			Parser parser = new Parser();
			AstProgram program = new AstProgram();
			Evaluator evaluator = new Evaluator();
	
			Repl repl = new Repl(parser, program, evaluator);
			repl.loop();
		}
	}
}