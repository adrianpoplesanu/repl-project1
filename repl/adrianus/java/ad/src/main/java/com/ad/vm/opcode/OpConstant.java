package com.ad.vm.opcode;

public class OpConstant extends OpCode {
    public OpConstant() {
        super(OpCodeEnum.OP_CONSTANT.getByte());
    }
}
