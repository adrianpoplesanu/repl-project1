package com.ad.evaluator;

import com.ad.ast.AstBoolean;
import com.ad.ast.AstExpressionStatement;
import com.ad.ast.AstIdentifier;
import com.ad.ast.AstInfixExpression;
import com.ad.ast.AstInteger;
import com.ad.ast.AstLetStatement;
import com.ad.ast.AstNode;
import com.ad.ast.AstPrefixExpression;
import com.ad.ast.AstProgram;
import com.ad.environment.Environment;
import com.ad.objects.AdBooleanObject;
import com.ad.objects.AdErrorObject;
import com.ad.objects.AdIntegerObject;
import com.ad.objects.AdObject;
import com.ad.objects.ObjectTypeEnum;

public class Evaluator {
	public static AdBooleanObject TrueObject = new AdBooleanObject(true);
	public static AdBooleanObject FalseObject = new AdBooleanObject(false);
	
    public AdObject eval(AstNode node, Environment env) {    	
    	switch(node.getType()) {
		case PROGRAM:
			return evalProgram((AstProgram)node, env);
		case BLOCK_STATEMENT:
			System.out.println("eval BlockStatement");
			break;
		case BOOLEAN:
			return evalBoolean(node, env);
		case CALL_EXPRESSION:
			System.out.println("eval CallExpression");
			break;
		case EXPRESSION_STATEMENT:
			return evalExpressionStatement(node, env);
		case FUNCTION_LITERAL:
			System.out.println("eval FunctionLiteral");
			break;
		case IDENTIFIER:
			return evalIdentifier(node, env);
		case IF_EXPRESSION:
			System.out.println("eval IfExpression");
			break;
		case INFIX_EXPRESSION:
			return evalInfixExpression(node, env);
		case INTEGER_LITERAL:
			return evalInteger(node, env);
		case LET_STATEMENT:
			return evalLetStatement(node, env);
		case PREFIX_EXPRESSION:
			AdObject right = eval(((AstPrefixExpression)node).getRight(), env);
			return evalPrefixExpression(((AstPrefixExpression)node).getOperator(), right);
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
    
    private AdObject evalProgram(AstProgram program, Environment env) {
    	for (AstNode stmt : program.statements) {
    		AdObject result = eval(stmt, env);
    		if (result != null) System.out.println(result.inspect());
    	}
    	return null;
    }
    
    private AdObject evalBoolean(AstNode node, Environment env) {
    	AstBoolean booleanLiteral = (AstBoolean)node;
    	return new AdBooleanObject(booleanLiteral.getValue());
    }
    
    private AdObject evalExpressionStatement(AstNode node, Environment env) {
    	AstExpressionStatement stmt = (AstExpressionStatement)node;
    	if (stmt.getExpression() != null) {
    		return eval(stmt.getExpression(), env);
    	}
    	return null;
    }
    
    private AdObject evalInteger(AstNode node, Environment env) {
    	AstInteger integerLiteral = (AstInteger)node;
    	return new AdIntegerObject(integerLiteral.getValue());
    }
    
    private AdObject evalPrefixExpression(String operator, AdObject right) {
    	if (operator == "!") {
    		return evalBangPrefixExpression(right);
    	}
    	if (operator == "-") {
    		return evalMinusPrefixExpression(right);
    	}
    	return newError("unknown prefix operator");
    }
    
    private AdObject evalBangPrefixExpression(AdObject right) {
    	return new AdBooleanObject(!((AdBooleanObject)right).getValue());
    }
    
    private AdObject evalMinusPrefixExpression(AdObject right) {
    	return new AdIntegerObject(-((AdIntegerObject)right).getValue());
    }
    
    private AdObject evalLetStatement(AstNode node, Environment env) {
		AstLetStatement letStatement = (AstLetStatement)node;
		AdObject obj = eval(letStatement.getValue(), env);
		env.set(letStatement.getName().getValue(), obj);
    	return null;
    }
    
    private AdObject evalIdentifier(AstNode node, Environment env) {
    	AstIdentifier ident = (AstIdentifier)node;
    	AdObject obj = env.get(ident.getValue());
    	return obj;
    }
    
    private AdObject evalInfixExpression(AstNode node, Environment env) {
    	AstInfixExpression expr = (AstInfixExpression)node;
    	AdObject left = eval(expr.getLeft(), env);
    	AdObject right = eval(expr.getRight(), env);
    	AdObject result = evalInfixExpression(expr.getOperator(), left, right);
    	return result;
    }
    
    private AdObject evalInfixExpression(String operator, AdObject left, AdObject right) {
    	if (left.getType() == ObjectTypeEnum.INT && right.getType() == ObjectTypeEnum.INT) {
    		return evalIntegerInfixExpression(operator, left, right);
    	}
    	if (left.getType() == ObjectTypeEnum.STRING && right.getType() == ObjectTypeEnum.STRING) {
    		return evalStringInfixExpression(operator, left, right);
    	}
    	return null;
    }
    
    private AdObject evalIntegerInfixExpression(String operator, AdObject left, AdObject right) {
    	int left_val = ((AdIntegerObject)left).getValue();
    	int right_val = ((AdIntegerObject)right).getValue();
    	switch (operator) {
    	case "+":
    		return new AdIntegerObject(left_val + right_val);
    	case "-":
    		return new AdIntegerObject(left_val - right_val);
    	case "*":
    		return new AdIntegerObject(left_val * right_val);
    	case "/":
    		return new AdIntegerObject(left_val / right_val);
    	case "<":
    		return new AdBooleanObject(left_val < right_val);
    	case ">":
    		return new AdBooleanObject(left_val > right_val);
    	case "<=":
    		return new AdBooleanObject(left_val <= right_val);
    	case ">=":
    		return new AdBooleanObject(left_val >= right_val);
    	}
    	return null;
    }
    
    private AdObject evalStringInfixExpression(String operator, AdObject left, AdObject right) {
    	return null;
    }
    
    private AdObject newError(String msg) {
    	return new AdErrorObject(msg);
    }
}
