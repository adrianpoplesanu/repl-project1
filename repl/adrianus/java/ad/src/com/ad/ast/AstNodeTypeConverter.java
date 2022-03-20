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
			put(AstNodeTypeEnum.ASSIGN_STATEMENT, "AssignStatement");
			put(AstNodeTypeEnum.DEF_STATEMENT, "DefStatement");
			put(AstNodeTypeEnum.INDEX_EXPRESSION, "IndexExpression");
			put(AstNodeTypeEnum.HASH_EXPRESSION, "HashExpression");
			put(AstNodeTypeEnum.NULL_EXPRESSION, "NullExpression");
			put(AstNodeTypeEnum.CLASS_STATEMENT, "ClassStatement");
			put(AstNodeTypeEnum.MEMBER_ACCESS, "MemberAccess");
			put(AstNodeTypeEnum.THIS_EXPRESSION, "ThisExpression");
			put(AstNodeTypeEnum.FOR_EXPRESSION, "ForExpression");
			put(AstNodeTypeEnum.POSTFIX_INCREMENT, "PostfixIncrement");
			put(AstNodeTypeEnum.PREFIX_INCREMENT, "PrefixIncrement");
		}
	};
	
	public static String convertToString(AstNodeTypeEnum ante) {
		return astNodeTypeMap.get(ante);
	}
}
