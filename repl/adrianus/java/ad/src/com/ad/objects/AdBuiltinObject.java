package com.ad.objects;

import com.ad.builtin.BuiltinInterface;

public class AdBuiltinObject extends AdObject {
	private BuiltinInterface builtinFunction;
	
	public AdBuiltinObject() {
		this.type = ObjectTypeEnum.BUILTIN;
	}

	public AdBuiltinObject(BuiltinInterface builtinFunction) {
		this.type = ObjectTypeEnum.BUILTIN;
		this.builtinFunction = builtinFunction;
	}
	
	@Override
	public String inspect() {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public void print() {
		// TODO Auto-generated method stub
		
	}

	public BuiltinInterface getBuiltinFunction() {
		return builtinFunction;
	}

	public void setBuiltinFunction(BuiltinInterface builtinFunction) {
		this.builtinFunction = builtinFunction;
	}
}
