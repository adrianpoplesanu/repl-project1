package com.ad.ast;

public class AstNodeUtils {
    public static void PrintASTNode(AstNode node, int level) {
    	if (node == null) return;
    	PrintLevel(level);
    	switch(node.getType()) {
    	    case PROGRAM:
    	    	System.out.println("->Program");
    	    	for (AstNode statement : ((AstProgram)node).statements) {
    	    		PrintASTNode(statement, level + 2);
    	    	}
    		break;
    	    case IDENTIFIER:
    	    	System.out.println("->Identifier " + ((AstIdentifier)node).getToken());
    	    break;
    	    case INTEGER_LITERAL:
    	    	System.out.println("->IntegerLiteral " + ((AstInteger)node).getToken());
    	    break;
    	    case LET_STATEMENT:
    	    	System.out.println("->LetStatement " + ((AstLetStatement)node).getToken() + " " + ((AstLetStatement)node).getName().getToken());
    	    	PrintASTNode(((AstLetStatement)node).getValue(), level + 2);
    	    break;
    	    case EXPRESSION_STATEMENT:
    	        System.out.println("->ExpressionStatement " + ((AstExpressionStatement)node).getToken());
    	        if (((AstExpressionStatement)node).getExpression() != null) {
    	        	PrintASTNode(((AstExpressionStatement)node).getExpression(), level + 2);
    	        }
    		break;
    		default:
    			System.out.println("not recognized node type for printing: " + AstNodeTypeConverter.convertToString(node.getType()));
    		break;
    	}
    }
    
    public static void PrintLevel(int size) {
    	for (int i = 0; i < size; i++) System.out.print(" ");
    }
}
