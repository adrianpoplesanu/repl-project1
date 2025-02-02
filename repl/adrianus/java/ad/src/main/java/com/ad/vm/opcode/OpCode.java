package com.ad.vm.opcode;

public class OpCode {
    protected byte code;

    public OpCode(byte code) {
        this.code = code;
    }

    @Override
    public String toString() {
        return "OpCode{" +
                "code=" + code +
                '}';
    }
}
