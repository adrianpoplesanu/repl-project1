package com.ad.environment;

import com.ad.objects.AdObject;

import java.util.HashMap;

public class BuiltinEnvironment {
    // this class should be an appendix to Environment
    // builtins should not live in Environment instances

    public HashMap<String, AdObject> store;

    public BuiltinEnvironment() {
        store = new HashMap<>();
    }

    public AdObject get(String key) {
        if (store.containsKey(key)) {
            return store.get(key);
        }
        return null;
    }

    public void set(String key, AdObject adObject) {
        store.put(key, adObject);
    }
}
