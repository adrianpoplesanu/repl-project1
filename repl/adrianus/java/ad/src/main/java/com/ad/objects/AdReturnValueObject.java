package com.ad.objects;

public class AdReturnValueObject extends AdObject {
	private AdObject returnValue;
	
	public AdReturnValueObject() {
		type = ObjectTypeEnum.RETURN_VALUE;
	}
	
	public AdReturnValueObject(AdObject obj) {
		this();
		returnValue = obj;
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

	@Override
	public String hash() {
		// return object, among other i might say, should not be hashable
		return null;
	}

	public AdObject getReturnValue() {
		return returnValue;
	}

	public void setReturnValue(AdObject returnValue) {
		this.returnValue = returnValue;
	}
}