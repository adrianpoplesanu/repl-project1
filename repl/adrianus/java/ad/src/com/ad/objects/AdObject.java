package com.ad.objects;

public abstract class AdObject {
	protected ObjectTypeEnum type;
	
	public abstract String inspect();
	public abstract void print();
	
	public ObjectTypeEnum getType() {
		return type;
	}
	
	public void setType(ObjectTypeEnum type) {
		this.type = type;
	}
}
