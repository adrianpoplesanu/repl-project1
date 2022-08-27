package com.ad.hash;

import com.ad.objects.AdObject;

public class HashPair<T> {
    private T key;
    private T value;

    public HashPair(T k, T v) {
        key = k;
        value = v;
    }

    public T getKey() {
        return key;
    }

    public void setKey(T key) {
        this.key = key;
    }

    public T getValue() {
        return value;
    }

    public void setValue(T value) {
        this.value = value;
    }
}
