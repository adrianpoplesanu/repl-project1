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
    OP_CURRENT_CLOSURE,

    OP_CLASS,
    OP_SET_METHOD,
    OP_INSTANTIATE,

    OP_GET_PROPERTY,
    OP_SET_PROPERTY,

    OP_GET_METHOD,
    OP_INVOKE,

    OP_SET_PROPERTY_SYM,
    OP_GET_PROPERTY_SYM
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

class OpJumpNotTruthy : public OpCode {
public:
    OpJumpNotTruthy() {
        byteCode = OP_JUMP_NOT_TRUTHY;
    }
};

class OpJump : public OpCode {
public:
    OpJump() {
        byteCode = OP_JUMP;
    }
};

class OpNull : public OpCode {
public:
    OpNull() {
        byteCode = OP_NULL;
    }
};

class OpGetGlobal : public OpCode {
public:
    OpGetGlobal() {
        byteCode = OP_GET_GLOBAL;
    }
};

class OpSetGlobal : public OpCode {
public:
    OpSetGlobal() {
        byteCode = OP_SET_GLOBAL;
    }
};

class OpArray : public OpCode {
public:
    OpArray() {
        byteCode = OP_ARRAY;
    }
};

class OpHash : public OpCode {
public:
    OpHash() {
        byteCode = OP_HASH;
    }
};

class OpIndexExpression : public OpCode {
public:
    OpIndexExpression() {
        byteCode = OP_INDEX_EXPRESSION;
    }
};

class OpCall : public OpCode {
public:
    OpCall() {
        byteCode = OP_CALL;
    }
};

class OpReturnValue : public OpCode {
public:
    OpReturnValue() {
        byteCode = OP_RETURN_VALUE;
    }
};

class OpReturn : public OpCode {
public:
    OpReturn() {
        byteCode = OP_RETURN;
    }
};

class OpGetLocal : public OpCode {
public:
    OpGetLocal() {
        byteCode = OP_GET_LOCAL;
    }
};
    
class OpSetLocal : public OpCode {
public:
    OpSetLocal() {
        byteCode = OP_SET_LOCAL;
    }
};

class OpGetBuiltin : public OpCode {
public:
    OpGetBuiltin() {
        byteCode = OP_GET_BUILTIN;
    }
};

class OpClosure : public OpCode {
public:
    OpClosure() {
        byteCode = OP_CLOSURE;
    }
};

class OpGetFree : public OpCode {
public:
    OpGetFree() {
        byteCode = OP_GET_FREE;
    }
};

class OpCurrentClosure : public OpCode {
public:
    OpCurrentClosure() {
        byteCode = OP_CURRENT_CLOSURE;
    }
};

class OpClass : public OpCode {
public:
    OpClass() {
        byteCode = OP_CLASS;
    }
};

class OpSetMethod : public OpCode {
public:
    OpSetMethod() {
        byteCode = OP_SET_METHOD;
    }
};

class OpInstantiate : public OpCode {
public:
    OpInstantiate() {
        byteCode = OP_INSTANTIATE;
    }
};

class OpGetProperty : public OpCode {
public:
    OpGetProperty() {
        byteCode = OP_GET_PROPERTY;
    }
};
    
class OpSetProperty : public OpCode {
public:
    OpSetProperty() {
        byteCode = OP_SET_PROPERTY;
    }
};

class OpGetMethod : public OpCode {
public:
    OpGetMethod() {
        byteCode = OP_GET_METHOD;
    }
};
    
class OpInvoke : public OpCode {
public:
    OpInvoke() {
        byteCode = OP_INVOKE;
    }
};

class OpSetPropertySym : public OpCode {
public:
    OpSetPropertySym() {
        byteCode = OP_SET_PROPERTY_SYM;
    }
};

class OpGetPropertySym : public OpCode {
public:
    OpGetPropertySym() {
        byteCode = OP_GET_PROPERTY_SYM;
    }
};

#endif