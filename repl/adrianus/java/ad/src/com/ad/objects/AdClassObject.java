package com.ad.objects;

public class AdClassObject extends AdObject {

    public AdClassObject() {
        type = ObjectTypeEnum.CLASS;
    }

    @Override
    public String inspect() {
        return "<class at memory address: " + Integer.toHexString(this.hashCode()) + ">";
    }

    @Override
    public void print() {
        System.out.println("Class object");
    }

    @Override
    public String hash() {
        return ObjectTypeConverter.convertToString(type) + inspect();
    }
}
