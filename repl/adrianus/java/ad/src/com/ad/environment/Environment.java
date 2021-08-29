package com.ad.environment;

import java.util.HashMap;

import com.ad.objects.AdObject;

public class Environment {
	private Environment outer;
	public static HashMap<String, AdObject> store;
	
	public Environment() {
		
	}
	
	public Environment(Environment outer) {
		this.outer = outer;
	}
	
	public boolean check(String key) {
		if (store.containsKey(key)) return true;
		return false;
	}
	
	public AdObject get(String key) {
		if (store.containsKey(key)) {
			return store.get(key);
		}
		if (outer.check(key)) return outer.get(key);
		return null;
	}
	
	public void set() {
		
	}
	
	public static Environment newEnvironment() {
		Environment env = new Environment();
		return env;
	}
	
	public static Environment newEnclosedEnvironment(Environment outer) {
		Environment env = new Environment(outer);
		return env;
	}
}
