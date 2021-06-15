import com.ad.lexer.Lexer;
import com.ad.token.Token;
import com.ad.token.TokenTypeEnum;

public class Ad {

	public static void main(String[] args) {
		System.out.println("Ad intepreter [JAVA]... v1.0");
		Lexer lexer = new Lexer();
		String source = "let a = 1;";
		lexer.reset(source);
		Token currentToken = lexer.nextToken();
		while(currentToken.getType() != TokenTypeEnum.EOF) {
			System.out.println(currentToken);
			currentToken = lexer.nextToken();
		}
	}
}
