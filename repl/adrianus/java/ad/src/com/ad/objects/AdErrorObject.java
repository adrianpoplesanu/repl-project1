package com.ad.objects;

public class AdErrorObject extends AdObject {
	private String msg;
	
	public AdErrorObject(String msg) {
		type = ObjectTypeEnum.ERROR;
		this.msg = msg;
	}

	@Override
	public String inspect() {
		return "ERROR: " + msg;
	}

	@Override
	public void print() {
		System.out.println("ERROR: " + msg);
	}

	public String getMsg() {
		return msg;
	}

	public void setMsg(String msg) {
		this.msg = msg;
	}
}
