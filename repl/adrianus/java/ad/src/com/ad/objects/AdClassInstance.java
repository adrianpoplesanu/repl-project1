package com.ad.objects;

import com.ad.environment.Environment;

public class AdClassInstance extends AdObject {
    private AdClassObject adClassObject;
    private String name;
    private Environment environment;

    public AdClassInstance() {
        type = ObjectTypeEnum.INSTANCE;
    }

    public AdClassInstance(String name, AdClassObject adClassObject, Environment environment) {
        this();
        this.name = name;
        this.adClassObject = adClassObject;
        this.environment = environment;
    }

    @Override
    public String inspect() {
        return "class instance at 0x0 :)";
    }

    @Override
    public void print() {
        System.out.println("TODO: implement this method");
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
}
