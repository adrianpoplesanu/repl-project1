package com.ad.vm.opcode;

public class OpSetLocal extends OpCode {
    public OpSetLocal() {
        super(OpCodeEnum.OP_SET_LOCAL.getByte());
    }
}
