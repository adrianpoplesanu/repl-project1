package com.ad.environment;

import java.util.HashMap;
import java.util.Map;

import com.ad.objects.AdObject;

public class Environment {
	private Environment outer;
	public HashMap<String, AdObject> store;
	
	public Environment() {
		store = new HashMap<String, AdObject>();
		outer = null;
	}
	
	public Environment(Environment outer) {
		this();
		this.outer = outer;
	}
	
	public boolean check(String key) {
		if (store.containsKey(key)) return true;
		if (outer != null && outer.check(key)) return true;
		return false;
	}
	
	public AdObject get(String key) {
		if (store.containsKey(key)) {
			return store.get(key);
		}
		if (outer != null && outer.check(key)) return outer.get(key);
		return null;
	}
	
	public void set(String key, AdObject value) {
		// ... si aici e problema
		/*if (outer != null && outer.check(key)) {
			outer.set(key, value);
		} else {
			store.put(key, value);
		}*/
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

	public Environment getOuter() {
		return outer;
	}

	public void setOuter(Environment outer) {
		this.outer = outer;
	}
}
