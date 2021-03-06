package com.ad.evaluator;

import com.ad.ast.AstNode;
import com.ad.environment.Environment;
import com.ad.objects.AdObject;

public class Evaluator {
    public AdObject eval(AstNode node, Environment env) {
    	switch(node.getType()) {
		case BLOCK_STATEMENT:
			System.out.println("eval BlockStatement");
			break;
		case BOOLEAN:
			System.out.println("eval Boolean");
			break;
		case CALL_EXPRESSION:
			System.out.println("eval CallExpression");
			break;
		case EXPRESSION_STATEMENT:
			System.out.println("eval ExpressionStatement");
			break;
		case FUNCTION_LITERAL:
			System.out.println("eval FunctionLiteral");
			break;
		case IDENTIFIER:
			System.out.println("eval Identifier");
			break;
		case IF_EXPRESSION:
			System.out.println("eval IfExpression");
			break;
		case INFIX_EXPRESSION:
			System.out.println("eval InfixExpression");
			break;
		case INTEGER_LITERAL:
			System.out.println("eval IntegerLiteral");
			break;
		case LET_STATEMENT:
			System.out.println("eval LetStatement");
			break;
		case PREFIX_EXPRESSION:
			System.out.println("eval PrefixExpression");
			break;
		case PROGRAM:
			System.out.println("eval Program");
			break;
		case RETURN_STATEMENT:
			System.out.println("eval ReturnStatement");
			break;
		case STRING_LITERAL:
			System.out.println("eval StringLiteral");
			break;
		case WHILE_EXPRESSION:
			System.out.println("eval WhileExpression");
			break;
		default:
			break;
    	}
    	return null;
    }
}
