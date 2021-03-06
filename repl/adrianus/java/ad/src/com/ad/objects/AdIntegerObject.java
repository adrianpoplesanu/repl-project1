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
	public void print() {
		System.out.println(value);
	}

}
