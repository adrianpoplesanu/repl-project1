package com.ad.objects;

public class AdIntegerObject extends AdObject {
	private int value;
	
	public AdIntegerObject() {
		type = ObjectTypeEnum.INT;
	}

	public AdIntegerObject(int value) {
		type = ObjectTypeEnum.INT;
		this.value = value;
	}
	
	@Override
	public String inspect() {
		return String.valueOf(value);
	}

	@Override
	public String repr() {
		return String.valueOf(value);
	}

	@Override
	public void print() {
		System.out.println(value);
	}

	@Override
	public String hash() {
		return ObjectTypeConverter.convertToString(type) + inspect();
	}

	public AdObject copy() {
		AdIntegerObject newObject = new AdIntegerObject(value);
		return newObject;
	}

	public int getValue() {
		return value;
	}

	public void setValue(int value) {
		this.value = value;
	}
}
