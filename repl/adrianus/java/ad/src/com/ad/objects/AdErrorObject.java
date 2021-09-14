package com.ad.objects;

public class AdErrorObject extends AdObject {
	private String msg;
	
	AdErrorObject(String msg) {
		this.msg = msg;
	}

	@Override
	public String inspect() {
		return "Error: " + msg;
	}

	@Override
	public void print() {
		System.out.println("Error: " + msg);
	}

	public String getMsg() {
		return msg;
	}

	public void setMsg(String msg) {
		this.msg = msg;
	}
}
