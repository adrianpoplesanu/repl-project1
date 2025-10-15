#ifndef AD_VM_OPCODE_H
#define AD_VM_OPCODE_H

enum OpCodeType {
    OP_CONSTANT,
    OP_ADD,
    OP_SUB,
    OP_MULTIPLY,
    OP_DIVIDE,
    OP_POP,
    OP_TRUE,
    OP_FALSE,
    OP_EQUAL,
    OP_NOTEQUAL,
    OP_GREATERTHAN,
    OP_GREATERTHAN_EQUAL,
    OP_MINUS,
    OP_BANG,
    OP_JUMP_NOT_TRUTHY,
    OP_JUMP,
    OP_NULL,
    OP_GET_GLOBAL,
    OP_SET_GLOBAL,
    OP_ARRAY,
    OP_HASH,
    OP_INDEX,
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
    OP_GET_PROPERTY_SYM,

    OP_PATCH_PROPERTY_SYM
};

class OpCode {
public:
    OpCodeType byteCode;
};

class OpConstant : public OpCode {
public:
    OpConstant() : OpCode() {
        byteCode = OP_CONSTANT;
    }
};

class OpAdd : public OpCode {
public:
    OpAdd() : OpCode() {
        byteCode = OP_ADD;
    }
};

class OpSub : public OpCode {
public:
    OpSub() : OpCode() {
        byteCode = OP_SUB;
    }
};

class OpMultiply : public OpCode {
public:
    OpMultiply() : OpCode() {
        byteCode = OP_MULTIPLY;
    }
};

class OpDivide : public OpCode {
public:
    OpDivide() : OpCode() {
        byteCode = OP_DIVIDE;
    }
};

class OpPop : public OpCode {
public:
    OpPop() : OpCode() {
        byteCode = OP_POP;
    }
};

class OpTrue : public OpCode {
public:
    OpTrue() : OpCode() {
        byteCode = OP_TRUE;
    }
};

class OpFalse : public OpCode {
public:
    OpFalse() : OpCode() {
        byteCode = OP_FALSE;
    }
};

class OpEqual : public OpCode {
public:
    OpEqual() : OpCode() {
        byteCode = OP_EQUAL;
    }
};

class OpNotEqual : public OpCode {
public:
    OpNotEqual() : OpCode() {
        byteCode = OP_NOTEQUAL;
    }
};

class OpGreaterThan : public OpCode {
public:
    OpGreaterThan() : OpCode() {
        byteCode = OP_GREATERTHAN;
    }
};

class OpGreaterThanEqual : public OpCode {
public:
    OpGreaterThanEqual() : OpCode() {
        byteCode = OP_GREATERTHAN_EQUAL;
    }
};

class OpMinus : public OpCode {
public:
    OpMinus() : OpCode() {
        byteCode = OP_MINUS;
    }
};

class OpBang : public OpCode {
public:
    OpBang() : OpCode() {
        byteCode = OP_BANG;
    }
};

class OpJumpNotTruthy : public OpCode {
public:
    OpJumpNotTruthy() : OpCode() {
        byteCode = OP_JUMP_NOT_TRUTHY;
    }
};

class OpJump : public OpCode {
public:
    OpJump() : OpCode() {
        byteCode = OP_JUMP;
    }
};

class OpNull : public OpCode {
public:
    OpNull() : OpCode() {
        byteCode = OP_NULL;
    }
};

class OpGetGlobal : public OpCode {
public:
    OpGetGlobal() : OpCode() {
        byteCode = OP_GET_GLOBAL;
    }
};

class OpSetGlobal : public OpCode {
public:
    OpSetGlobal() : OpCode() {
        byteCode = OP_SET_GLOBAL;
    }
};

class OpArray : public OpCode {
public:
    OpArray() : OpCode() {
        byteCode = OP_ARRAY;
    }
};

class OpHash : public OpCode {
public:
    OpHash() : OpCode() {
        byteCode = OP_HASH;
    }
};

class OpIndex : public OpCode {
public:
    OpIndex() : OpCode() {
        byteCode = OP_INDEX;
    }
};

class OpCall : public OpCode {
public:
    OpCall() : OpCode() {
        byteCode = OP_CALL;
    }
};

class OpReturnValue : public OpCode {
public:
    OpReturnValue() : OpCode() {
        byteCode = OP_RETURN_VALUE;
    }
};

class OpReturn : public OpCode {
public:
    OpReturn() : OpCode() {
        byteCode = OP_RETURN;
    }
};

class OpGetLocal : public OpCode {
public:
    OpGetLocal() : OpCode() {
        byteCode = OP_GET_LOCAL;
    }
};
    
class OpSetLocal : public OpCode {
public:
    OpSetLocal() : OpCode() {
        byteCode = OP_SET_LOCAL;
    }
};

class OpGetBuiltin : public OpCode {
public:
    OpGetBuiltin() : OpCode() {
        byteCode = OP_GET_BUILTIN;
    }
};

class OpClosure : public OpCode {
public:
    OpClosure() : OpCode() {
        byteCode = OP_CLOSURE;
    }
};

class OpGetFree : public OpCode {
public:
    OpGetFree() : OpCode() {
        byteCode = OP_GET_FREE;
    }
};

class OpCurrentClosure : public OpCode {
public:
    OpCurrentClosure() : OpCode() {
        byteCode = OP_CURRENT_CLOSURE;
    }
};

class OpClass : public OpCode {
public:
    OpClass() : OpCode() {
        byteCode = OP_CLASS;
    }
};

class OpSetMethod : public OpCode {
public:
    OpSetMethod() : OpCode() {
        byteCode = OP_SET_METHOD;
    }
};

class OpInstantiate : public OpCode {
public:
    OpInstantiate() : OpCode() {
        byteCode = OP_INSTANTIATE;
    }
};

class OpGetProperty : public OpCode {
public:
    OpGetProperty() : OpCode() {
        byteCode = OP_GET_PROPERTY;
    }
};
    
class OpSetProperty : public OpCode {
public:
    OpSetProperty() : OpCode() {
        byteCode = OP_SET_PROPERTY;
    }
};

class OpGetMethod : public OpCode {
public:
    OpGetMethod() : OpCode() {
        byteCode = OP_GET_METHOD;
    }
};
    
class OpInvoke : public OpCode {
public:
    OpInvoke() : OpCode() {
        byteCode = OP_INVOKE;
    }
};

class OpSetPropertySym : public OpCode {
public:
    OpSetPropertySym() : OpCode() {
        byteCode = OP_SET_PROPERTY_SYM;
    }
};

class OpGetPropertySym : public OpCode {
public:
    OpGetPropertySym() : OpCode() {
        byteCode = OP_GET_PROPERTY_SYM;
    }
};

class OpPatchPropertySym : public OpCode {
public:
    OpPatchPropertySym() : OpCode() {
        byteCode = OP_PATCH_PROPERTY_SYM;
    }
};

#endif