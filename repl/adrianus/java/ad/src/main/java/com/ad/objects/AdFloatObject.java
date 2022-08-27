package com.ad.objects;

public class AdFloatObject extends AdObject {
    private float value;

    public AdFloatObject() {
        type = ObjectTypeEnum.FLOAT;
    }

    public AdFloatObject(float value) {
        this();
        this.value = value;
    }

    @Override
    public String inspect() {
        return String.valueOf(value);
    }

    @Override
    public void print() {
        System.out.println(value);
    }

    @Override
    public String hash() {
        return ObjectTypeConverter.convertToString(type) + inspect();
    }

    public float getValue() {
        return value;
    }

    public void setValue(float value) {
        this.value = value;
    }
}
