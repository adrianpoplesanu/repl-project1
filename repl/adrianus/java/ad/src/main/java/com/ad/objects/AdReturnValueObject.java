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
		//return null;
		//return returnValue.inspect(); // TODO: asta e implementarea din cpp
		return "WARNING: return outside function";
	}

	@Override
	public String repr() {
		return "WARNING: return outside function";
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
