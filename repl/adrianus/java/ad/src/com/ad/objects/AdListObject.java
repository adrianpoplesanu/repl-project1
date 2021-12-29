package com.ad.objects;

import java.util.List;

public class AdListObject extends AdObject {
    private List<AdObject> elements;

    public AdListObject() {
        type = ObjectTypeEnum.LIST;
    }

    public AdListObject(List<AdObject> e) {
        this();
        elements = e;
    }

    @Override
    public String inspect() {
        return "[" + "]";
    }

    @Override
    public void print() {
        System.out.println("TODO: implement print() in AdListObject");
    }

    public List<AdObject> getElements() {
        return elements;
    }

    public void setElements(List<AdObject> elements) {
        this.elements = elements;
    }
}
