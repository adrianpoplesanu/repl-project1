package com.ad.ast;

import java.util.HashMap;

public class AstNodeTypeConverter {
	public static HashMap<AstNodeTypeEnum, String> astNodeTypeMap = new HashMap<AstNodeTypeEnum, String>() {
		private static final long serialVersionUID = 1L;
		{
			put(AstNodeTypeEnum.PROGRAM, "Program");
			put(AstNodeTypeEnum.LET_STATEMENT, "LetStatement");
			put(AstNodeTypeEnum.RETURN_STATEMENT, "ReturnStatement");
			put(AstNodeTypeEnum.IDENTIFIER, "Identifier");
			put(AstNodeTypeEnum.INTEGER_LITERAL, "IntegerLiteral");
			put(AstNodeTypeEnum.BOOLEAN, "Boolean");
			put(AstNodeTypeEnum.EXPRESSION_STATEMENT, "ExpressionStatement");
			put(AstNodeTypeEnum.INFIX_EXPRESSION, "InfixExpression");
			put(AstNodeTypeEnum.PREFIX_EXPRESSION, "PrefixExpression");
			put(AstNodeTypeEnum.IF_EXPRESSION, "IfExpression");
			put(AstNodeTypeEnum.BLOCK_STATEMENT, "BlockStatement");
			put(AstNodeTypeEnum.FUNCTION_LITERAL, "FunctionLiteral");
			put(AstNodeTypeEnum.CALL_EXPRESSION, "CallExpression");
			put(AstNodeTypeEnum.WHILE_EXPRESSION, "WhileExpression");
		}
	};
	
	public static String convertToString(AstNodeTypeEnum ante) {
		return astNodeTypeMap.get(ante);
	}
}
