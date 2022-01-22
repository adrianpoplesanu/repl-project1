package com.ad.objects;

import com.ad.builtin.BuiltinInterface;

public class AdBuiltinObject extends AdObject {
	private BuiltinInterface builtinFunction;
	
	public AdBuiltinObject() {
		this.type = ObjectTypeEnum.BUILTIN;
	}

	public AdBuiltinObject(BuiltinInterface builtinFunction) {
		this();
		this.builtinFunction = builtinFunction;
	}
	
	@Override
	public String inspect() {
		return "<funtion at memory address: 0x" + Integer.toHexString(this.hashCode()) + ">";
	}

	@Override
	public void print() {
		System.out.println("TODO: implement print() in AdBuiltinObject");
	}

	@Override
	public String hash() {
		return ObjectTypeConverter.convertToString(type) + inspect();
	}

	public BuiltinInterface getBuiltinFunction() {
		return builtinFunction;
	}

	public void setBuiltinFunction(BuiltinInterface builtinFunction) {
		this.builtinFunction = builtinFunction;
	}
}
