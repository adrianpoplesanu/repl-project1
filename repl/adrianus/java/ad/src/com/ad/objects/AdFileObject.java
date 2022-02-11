package com.ad.objects;

public class AdFileObject extends AdObject {
    private String filename;
    private String operator;

    public AdFileObject(String filename, String operator) {
        this.filename = filename;
        this.operator = operator;
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
        return null;
    }
}
