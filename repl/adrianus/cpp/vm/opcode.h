#ifndef __OPCODE_H
#define __OPCODE_H

enum OpCodeType {
    OP_CONSTANT,
    OP_ADD,
    OP_SUB,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_POP,
    OP_TRUE,
    OP_FALSE
};

class OpCode {
public:
    OpCodeType byteCode;
};

class OpConstant : public OpCode {
public:
    OpConstant() {
        byteCode = OP_CONSTANT;
    }
};

class OpAdd : public OpCode {
public:
    OpAdd() {
        byteCode = OP_ADD;
    }
};

class OpSub : public OpCode {
public:
    OpSub() {
        byteCode = OP_SUB;
    }
};

class OpMultiply : public OpCode {
public:
    OpMultiply() {
        byteCode = OP_MULTIPLY;
    }
};

class OpDivide : public OpCode {
public:
    OpDivide() {
        byteCode = OP_DIVIDE;
    }
};

class OpPop : public OpCode {
public:
    OpPop() {
        byteCode = OP_POP;
    }
};

class OpTrue : public OpCode {
public:
    OpTrue() {
        byteCode = OP_TRUE;
    }
};

class OpFalse : public OpCode {
public:
    OpFalse() {
        byteCode = OP_FALSE;
    }
};

#endif