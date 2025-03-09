package com.ad.vm;

import java.util.Arrays;

public class Instructions {
    private byte[] bytes;
    private int n = 0;

    public int size() {
        return bytes.length;
    }

    public byte get(int i) {
        return bytes[i];
    }

    public void add(byte instruction) {
        bytes[n++] = instruction;
    }

    @Override
    public String toString() {
        return "Instructions{" +
                "bytes=" + Arrays.toString(bytes) +
                ", n=" + n +
                '}';
    }
}
