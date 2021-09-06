package com.ad.objects;

public class AdBooleanObject extends AdObject {
	private boolean value;
	
    public AdBooleanObject(boolean value) {
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
