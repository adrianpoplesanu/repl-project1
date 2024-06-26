package com.ad.objects;

import com.ad.hash.HashPair;

import java.util.HashMap;
import java.util.Map;

public class AdHashObject extends AdObject {
    private HashMap<String, HashPair<AdObject>> elements;

    public AdHashObject() {
        type = ObjectTypeEnum.HASH;
    }

    public AdHashObject(HashMap<String, HashPair<AdObject>> e) {
        this();
        elements = e;
    }

    @Override
    public String inspect() {
        String out = "{";
        boolean first = true;
        for (Map.Entry<String, HashPair<AdObject>> e : elements.entrySet()) {
            if (!first) out += ", ";
            out += e.getValue().getKey().inspect() + ": " + e.getValue().getValue().inspect();
            first = false;
        }
        out += "}";
        return out;
    }

    @Override
    public void print() {

    }

    @Override
    public String hash() {
        return ObjectTypeConverter.convertToString(type) + inspect();
    }

    @Override
    public AdObject copy() {
        HashMap<String, HashPair<AdObject>> newElements = new HashMap<>();
        for (Map.Entry<String, HashPair<AdObject>> element : elements.entrySet()) {
            newElements.put(element.getKey(),
                    new HashPair<>(
                            element.getValue().getKey().copy(),
                            element.getValue().getValue().copy()));
        }
        AdHashObject newObject = new AdHashObject(newElements);
        return newObject;
    }

    public HashMap<String, HashPair<AdObject>> getElements() {
        return elements;
    }

    public void setElements(HashMap<String, HashPair<AdObject>> elements) {
        this.elements = elements;
    }
}
