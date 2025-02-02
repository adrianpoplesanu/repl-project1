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
    OP_FALSE,
    OP_EQUALS,
    OP_NOTEQUALS,
    OP_GREATERTHAN,
    OP_GREATERTHANEQUAL,
    OP_MINUS,
    OP_BANG,
    OP_JUMP_NOT_TRUTHY,
    OP_JUMP,
    OP_NULL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_ARRAY,
    OP_HASH,
    OP_INDEX_EXPRESSION,
    OP_CALL,
    OP_RETURN_VALUE,
    OP_RETURN,
    OP_GET_LOCAL,
    OP_SET_LOCAL,
    OP_GET_BUILTIN,
    OP_CLOSURE,
    OP_GET_FREE,
    OP_CURRENT_CLOSURE
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

class OpEquals : public OpCode {
public:
    OpEquals() {
        byteCode = OP_EQUALS;
    }
};

class OpNotEquals : public OpCode {
public:
    OpNotEquals() {
        byteCode = OP_NOTEQUALS;
    }
};

class OpGreaterThan : public OpCode {
public:
    OpGreaterThan() {
        byteCode = OP_GREATERTHAN;
    }
};

class OpGreaterThanEqual : public OpCode {
public:
    OpGreaterThanEqual() {
        byteCode = OP_GREATERTHANEQUAL;
    }
};

class OpMinus : public OpCode {
public:
    OpMinus() {
        byteCode = OP_MINUS;
    }
};

class OpBang : public OpCode {
public:
    OpBang() {
        byteCode = OP_BANG;
    }
};

#endif