package com.ad.vm.opcode;


public enum OpCodeEnum {
    OP_CONSTANT(0),
    OP_ADD(1),
    OP_SUB(2),
    OP_MUL(3),
    OP_DIVIDE(4);

    public final int code;

    OpCodeEnum(int code) {
        this.code = code;
    }

    public byte getByte() {
        return (byte) code;
    }
}
