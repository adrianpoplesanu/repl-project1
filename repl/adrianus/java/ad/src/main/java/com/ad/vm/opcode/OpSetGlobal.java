package com.ad.vm.opcode;

public class OpSetGlobal extends OpCode {
    public OpSetGlobal() {
        super(OpCodeEnum.OP_SET_GLOBAL.getByte());
    }
}
