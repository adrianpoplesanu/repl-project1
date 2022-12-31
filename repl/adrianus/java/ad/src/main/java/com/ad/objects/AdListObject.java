package com.ad.objects;

import java.util.ArrayList;
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
        String out = "[";
        boolean first = true;
        for (AdObject obj : elements) {
            if (!first) out += ", ";
            out += obj.inspect();
            first = false;
        }
        out += "]";
        return out;
    }

    @Override
    public void print() {
        System.out.println("TODO: implement print() in AdListObject");
    }

    @Override
    public String hash() {
        return ObjectTypeConverter.convertToString(type) + inspect();
    }

    public AdObject copy() {
        List<AdObject> newElements = new ArrayList<>();
        for (AdObject e : elements) {
            newElements.add(e.copy());
        }
        AdListObject newList = new AdListObject(newElements);
        return newList;
    }

    public List<AdObject> getElements() {
        return elements;
    }

    public void setElements(List<AdObject> elements) {
        this.elements = elements;
    }
}
