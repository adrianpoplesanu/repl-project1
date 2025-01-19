package com.ad.vm.opcode;

public class OpClosure extends OpCode {
    public OpClosure() {
        super(OpCodeEnum.OP_CLOSURE.getByte());
    }
}
