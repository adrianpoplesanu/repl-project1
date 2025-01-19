package com.ad.vm.opcode;


public enum OpCodeEnum {
    OP_CONSTANT(0),
    OP_ADD(1),
    OP_SUB(2),
    OP_MUL(3),
    OP_DIVIDE(4),
    OP_POP(5),
    OP_TRUE(6),
    OP_FALSE(7),
    OP_EQUAL(8),
    OP_NOTEQUAL(9),
    OP_GREATERTHAN(10),
    OP_GREATERTHAN_EQUAL(11),
    OP_BANG (12),
    OP_MINUS(13),
    OP_JUMP_NOT_TRUTHY(14),
    OP_JUMP(15),
    OP_NULL(16),
    OP_GET_GLOBAL(17),
    OP_SET_GLOBAL(18),
    OP_ARRAY(19),
    OP_HASH(20),
    OP_INDEX(21),
    OP_CALL(22),
    OP_RETURN_VALUE(23),
    OP_RETURN(24),
    OP_GET_LOCAL(25),
    OP_SET_LOCAL(26),
    OP_GET_BUILTIN(27),
    OP_CLOSURE(28),
    OP_GET_FREE(29),
    OP_CURRENT_CLOSURE(30);

    public final int code;

    OpCodeEnum(int code) {
        this.code = code;
    }

    public byte getByte() {
        return (byte) code;
    }
}
