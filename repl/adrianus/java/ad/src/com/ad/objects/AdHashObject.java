package com.ad.objects;

import java.util.HashMap;

public class AdHashObject extends AdObject {
    private HashMap<String, AdObject> elements;

    public AdHashObject() {
        type = ObjectTypeEnum.HASH;
    }

    public AdHashObject(HashMap<String, AdObject> e) {
        this();
        elements = e;
    }

    @Override
    public String inspect() {
        return null;
    }

    @Override
    public void print() {

    }

    @Override
    public String hash() {
        return ObjectTypeConverter.convertToString(type) + inspect();
    }
}
