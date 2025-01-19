package com.ad.vm.opcode;

public class OpJumpNotThruthy extends OpCode {
    public OpJumpNotThruthy() {
        super(OpCodeEnum.OP_JUMP_NOT_TRUTHY.getByte());
    }
}
