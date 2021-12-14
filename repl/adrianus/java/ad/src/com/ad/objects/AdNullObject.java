package com.ad.objects;

public class AdNullObject extends AdObject {

    public AdNullObject() {
        type = ObjectTypeEnum.NULL;
    }

    @Override
    public String inspect() {
        return "null";
    }

    @Override
    public void print() {
        System.out.println("NullObject");
    }
}
