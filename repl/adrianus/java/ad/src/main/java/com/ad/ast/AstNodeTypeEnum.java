package com.ad.ast;

public enum AstNodeTypeEnum {
	PROGRAM,
	LET_STATEMENT,
	RETURN_STATEMENT,
	BREAK_STATEMENT,
	CONTINUE_STATEMENT,
	IDENTIFIER,
	INTEGER_LITERAL,
	FLOAT_LITERAL,
	EXPRESSION_STATEMENT,
	PREFIX_EXPRESSION,
	INFIX_EXPRESSION,
	BOOLEAN,
	IF_EXPRESSION,
	BLOCK_STATEMENT,
	FUNCTION_LITERAL,
	CALL_EXPRESSION,
	STRING_LITERAL,
	WHILE_EXPRESSION,
	ASSIGN_STATEMENT,
	DEF_STATEMENT,
	LIST_EXPRESSION,
	INDEX_EXPRESSION,
	HASH_EXPRESSION,
	MEMBER_ACCESS,
	CLASS_STATEMENT,
	PREFIX_INCREMENT,
	POSTFIX_INCREMENT,
	COMMENT,
	FOR_EXPRESSION,
	NULL_EXPRESSION,
	THIS_EXPRESSION
}