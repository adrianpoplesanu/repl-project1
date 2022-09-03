package com.ad.environment;

import com.ad.objects.AdObject;

import java.util.HashMap;

public class BuiltinEnvironment implements EnvironmentInterface {
    // TODO: do not use this, rather create a new Environment reference in
    //  Environment that will point to an Environment that only holds
    //  bootstrap objects
    // this class should be an appendix to Environment
    // builtins should not live in Environment instances

    public HashMap<String, AdObject> store;

    public BuiltinEnvironment() {
        store = new HashMap<>();
    }

    @Override
    public AdObject get(String key) {
        if (store.containsKey(key)) {
            return store.get(key);
        }
        return null;
    }

    @Override
    public void set(String key, AdObject adObject) {
        store.put(key, adObject);
    }

    @Override
    public boolean check(String key) {
        return false;
    }

    @Override
    public void setLocalParam(String key, AdObject adObject) {

    }

    @Override
    public Environment getOuter() {
        return null;
    }

    @Override
    public void setOuter(Environment outer) {

    }
}
