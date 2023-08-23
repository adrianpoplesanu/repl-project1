package com.ad.objects;

import com.ad.builtin.AbstractBuiltin;
import com.ad.builtin.BuiltinInterface;

public class AdBuiltinObject extends AdObject {
	private AbstractBuiltin builtinFunction;
	
	public AdBuiltinObject() {
		this.type = ObjectTypeEnum.BUILTIN;
	}

	public AdBuiltinObject(AbstractBuiltin builtinFunction) {
		this();
		this.builtinFunction = builtinFunction;
	}
	
	@Override
	public String inspect() {
		return "<built-in at memory address: 0x" + Integer.toHexString(this.hashCode()) + ">";
	}

	@Override
	public void print() {
		System.out.println("TODO: implement print() in AdBuiltinObject");
	}

	@Override
	public String hash() {
		return ObjectTypeConverter.convertToString(type) + inspect();
	}

	public AbstractBuiltin getBuiltinFunction() {
		return builtinFunction;
	}

	public void setBuiltinFunction(AbstractBuiltin builtinFunction) {
		this.builtinFunction = builtinFunction;
	}
}
