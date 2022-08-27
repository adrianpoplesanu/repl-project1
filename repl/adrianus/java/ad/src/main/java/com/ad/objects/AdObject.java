package com.ad.objects;

public abstract class AdObject {
	protected ObjectTypeEnum type;
	
	public abstract String inspect();
	public abstract void print();
	public abstract String hash();
	// public abstract String repr();

	public ObjectTypeEnum getType() {
		return type;
	}
	
	public void setType(ObjectTypeEnum type) {
		this.type = type;
	}
}
