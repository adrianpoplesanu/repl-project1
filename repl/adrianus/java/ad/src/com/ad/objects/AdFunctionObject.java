package com.ad.objects;

import java.util.ArrayList;

import com.ad.ast.AstNode;
import com.ad.environment.Environment;

public class AdFunctionObject extends AdObject {
    private AstNode block;
    private ArrayList<AstNode> parameters;
    private Environment env;
    
    public AdFunctionObject(ArrayList<AstNode> p, AstNode b, Environment e) {
    	type = ObjectTypeEnum.FUNCTION;
    	parameters = p;
    	block = b;
    	env = e;
    }

	@Override
	public String inspect() {
		return "<funtion at memory address: 0x" + Integer.toHexString(this.hashCode()) + ">";
	}

	@Override
	public void print() {
		System.out.println("Function object");
	}

	@Override
	public String hash() {
		return ObjectTypeConverter.convertToString(type) + inspect();
	}

	public AstNode getBlock() {
		return block;
	}

	public void setBlock(AstNode block) {
		this.block = block;
	}

	public ArrayList<AstNode> getParameters() {
		return parameters;
	}

	public void setParameters(ArrayList<AstNode> parameters) {
		this.parameters = parameters;
	}

	public Environment getEnv() {
		return env;
	}

	public void setEnv(Environment env) {
		this.env = env;
	}
}
