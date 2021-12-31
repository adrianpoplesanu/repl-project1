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
}
