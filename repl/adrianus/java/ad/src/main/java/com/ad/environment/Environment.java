package com.ad.environment;

import java.util.HashMap;
import java.util.Map;

import com.ad.objects.AdObject;

public class Environment {
	private Environment bootstrap;
	private Environment outer;
	public HashMap<String, AdObject> store;
	
	public Environment() {
		store = new HashMap<>();
		outer = null;
		bootstrap = null;
	}
	
	public Environment(Environment outer) {
		this();
		this.outer = outer;
	}

	public Environment(Environment outer, Environment bootstrap) {
		this();
		this.outer = outer;
		this.bootstrap = bootstrap;
	}
	
	public boolean check(String key) {
		if (store.containsKey(key)) return true;
		if (outer != null && outer.check(key)) return true;
		if (bootstrap != null && bootstrap.check(key)) return true;
		return false;
	}
	
	public AdObject get(String key) {
		if (store.containsKey(key)) {
			return store.get(key);
		}
		if (outer != null && outer.check(key)) return outer.get(key);
		if (bootstrap != null && bootstrap.check(key)) return bootstrap.get(key);
		return null;
	}
	
	public void set(String key, AdObject value) {
		if (store.containsKey(key)) {
			store.put(key, value);
		} else {
			if (outer != null && outer.check(key)) {
				outer.set(key, value);
			} else {
				store.put(key, value);
			}
		}
	}

	public void setLocalParam(String key, AdObject value) {
		store.put(key, value);
	}

	public Environment getOuter() {
		return outer;
	}

	public void setOuter(Environment outer) {
		this.outer = outer;
	}

	public Environment getBootstrap() {
		return bootstrap;
	}

	public void setBootstrap(Environment bootstrap) {
		this.bootstrap = bootstrap;
	}

	public String toString() {
		String out = "{";
		int total = store.size();
		int i = 0;
		for (Map.Entry<String, AdObject> entry : store.entrySet()) {
			out += entry.getKey() + ": " + entry.getValue().inspect();
			i++;
			if (i < total) out += ", ";
		}
		out += "}";
		return out;
	}
}
