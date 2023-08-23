package com.ad.objects;

public class AdFileObject extends AdObject {
    private String filename;
    private String operator;

    public AdFileObject() {
        type = ObjectTypeEnum.FILE;
    }

    public AdFileObject(String filename, String operator) {
        this();
        this.filename = filename;
        this.operator = operator;
    }

    @Override
    public String inspect() {
        return "<file object at address: 0x" + Integer.toHexString(this.hashCode()) + ">";
    }

    @Override
    public void print() {

    }

    @Override
    public String hash() {
        return null;
    }

    public String getFilename() {
        return filename;
    }

    public void setFilename(String filename) {
        this.filename = filename;
    }

    public String getOperator() {
        return operator;
    }

    public void setOperator(String operator) {
        this.operator = operator;
    }
}
