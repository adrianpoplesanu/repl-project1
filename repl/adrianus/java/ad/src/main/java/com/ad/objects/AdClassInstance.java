package com.ad.objects;

import com.ad.environment.Environment;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;

public class AdClassInstance extends AdObject {
    private AdClassObject adClassObject;
    private String name;
    private Environment environment;
    private HashMap<String, Environment> inheritedEnvs;
    private List<String> inheritFrom;

    public AdClassInstance() {
        type = ObjectTypeEnum.INSTANCE;
    }

    public AdClassInstance(String name, AdClassObject adClassObject, Environment environment) {
        this();
        this.name = name;
        this.adClassObject = adClassObject;
        this.environment = environment;
        inheritedEnvs = new HashMap<>();
        inheritFrom = new ArrayList<>();
    }

    @Override
    public String inspect() {
        return "<class instance at memory address: 0x" + Integer.toHexString(this.hashCode()) + ">";
    }

    @Override
    public void print() {
        System.out.println("ClassInstance object");
    }

    @Override
    public String hash() {
        return null;
    }

    public AdClassObject getAdClassObject() {
        return adClassObject;
    }

    public void setAdClassObject(AdClassObject adClassObject) {
        this.adClassObject = adClassObject;
    }

    public String getName() {
        return name;
    }

    public void setName(String name) {
        this.name = name;
    }

    public Environment getEnvironment() {
        return environment;
    }

    public void setEnvironment(Environment environment) {
        this.environment = environment;
    }

    public HashMap<String, Environment> getInheritedEnvs() {
        return inheritedEnvs;
    }

    public void setInheritedEnvs(HashMap<String, Environment> inheritedEnvs) {
        this.inheritedEnvs = inheritedEnvs;
    }

    public List<String> getInheritFrom() {
        return inheritFrom;
    }

    public void setInheritFrom(List<String> inheritFrom) {
        this.inheritFrom = inheritFrom;
    }
}
