package com.ad.vm.opcode;

public class OpGetLocal extends OpCode {
    public OpGetLocal() {
        super(OpCodeEnum.OP_GET_LOCAL.getByte());
    }
}
