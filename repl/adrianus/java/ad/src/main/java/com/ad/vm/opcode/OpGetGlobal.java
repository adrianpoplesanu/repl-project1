package com.ad.vm.opcode;

public class OpGetGlobal extends OpCode {
    public OpGetGlobal() {
        super(OpCodeEnum.OP_GET_GLOBAL.getByte());
    }
}
