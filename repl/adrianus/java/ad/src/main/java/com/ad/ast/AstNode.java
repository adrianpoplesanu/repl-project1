package com.ad.ast;

public abstract class AstNode {
	protected AstNodeTypeEnum type;
	
	public abstract String tokenLiteral();
	public abstract String toString();

	public AstNodeTypeEnum getType() {
		return type;
	}

	public void setType(AstNodeTypeEnum type) {
		this.type = type;
	}
}
