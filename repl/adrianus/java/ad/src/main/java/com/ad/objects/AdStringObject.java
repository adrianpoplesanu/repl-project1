package com.ad.objects;

import com.ad.token.Token;

public class AdStringObject extends AdObject {
	private String value;
	
	public AdStringObject() {
		type = ObjectTypeEnum.STRING;
		value = "";
	}
	
	public AdStringObject(String value) {
		type = ObjectTypeEnum.STRING;
		this.value = value;
	}

	@Override
	public String inspect() {
		return "'" + value + "'";
	}

	@Override
	public void print() {
		// TODO Auto-generated method stub
		
	}

	@Override
	public String hash() {
		return ObjectTypeConverter.convertToString(type) + inspect();
	}

	@Override
	public AdObject copy() {
		AdStringObject newObject = new AdStringObject(value);
		return newObject;
	}

	public String getValue() {
		return value;
	}

	public void setValue(String value) {
		this.value = value;
	}
}
