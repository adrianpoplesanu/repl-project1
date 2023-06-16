package com.ad.environment;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import com.ad.objects.AdObject;

public class Environment {
	private Environment bootstrap;
	private Environment outer;
	public HashMap<String, AdObject> store;
	public HashMap<String, Environment> siblings;
	private boolean isInstanceEnvironment;
	private AdObject owningInstanceEnvironment;

	public Environment() {
		store = new HashMap<>();
		siblings = new HashMap<>();
		outer = null;
		bootstrap = null;
		isInstanceEnvironment = false;
		owningInstanceEnvironment = null;
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

	public AdObject lookupOnlyInStore(String key) {
		return store.get(key);
	}

	public AdObject lookupConstructor() {
		// constructor needs to be specific to current store
		// the normal lookup mechanism searches for "constructor" identifier in store and then recursive in outer
		// this means that if no constructor in current class then the higher up class that nests the current one
		// will hit a constructor on the env.get if one is present
		// search for "constructor" only in local env, I don't want the constructor of a higher class
		return lookupOnlyInStore("constructor");
	}

	public Environment getSibling(String ident) {
		// used for super() on inherited classes
		return siblings.get(ident);
	}

	public void addSibling(String ident, Environment env) {
		// used for super() on inherited classes
		siblings.put(ident, env);
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

	public boolean isInstanceEnvironment() {
		return isInstanceEnvironment;
	}

	public void setInstanceEnvironment(boolean instanceEnvironment) {
		isInstanceEnvironment = instanceEnvironment;
	}

	public AdObject getOwningInstanceEnvironment() {
		return owningInstanceEnvironment;
	}

	public void setOwningInstanceEnvironment(AdObject owningInstanceEnvironment) {
		this.owningInstanceEnvironment = owningInstanceEnvironment;
	}

	public List<String> populateGetattrs() {
		List<String> result = new ArrayList<>();
		store.keySet().stream().forEach(key -> result.add(key));
		return result;
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
