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
    OP_SET_INDEX,
    /// Stack: ... old_value, new_value, left, index (index on top). Sets left[index]=new_value, pushes old_value.
    OP_POSTFIX_INDEX,
    /// Stack: ... left, start, end, step (step on top). NULL operands mean omitted slice bounds.
    OP_SLICE,
    /// Stack: ... value, left, index (index on top). Sets left[index]=value (for compound index assign).
    OP_PATCH_INDEX,
    OP_CALL,
    OP_CALL_KW,
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

    OP_PATCH_PROPERTY_SYM,

    OP_GET_THIS,

    OP_GET_SUPER_METHOD,

    /// Pop the top-of-stack value and print it like `Evaluator::EvalProgram` does for each
    /// top-level statement (skip only OBJ_SIGNAL). Used for VM file/REPL parity with the evaluator.
    OP_FILE_STMT_OUTPUT
};

class OpCode {
public:
    OpCodeType byteCode;
    OpCode(OpCodeType byteCode) : byteCode(byteCode) {}
};

class OpConstant : public OpCode {
public:
    OpConstant() : OpCode(OP_CONSTANT) {
    }
};

class OpAdd : public OpCode {
public:
    OpAdd() : OpCode(OP_ADD) {
    }
};

class OpSub : public OpCode {
public:
    OpSub() : OpCode(OP_SUB) {
    }
};

class OpMultiply : public OpCode {
public:
    OpMultiply() : OpCode(OP_MULTIPLY) {
    }
};

class OpDivide : public OpCode {
public:
    OpDivide() : OpCode(OP_DIVIDE) {
    }
};

class OpPop : public OpCode {
public:
    OpPop() : OpCode(OP_POP) {
    }
};

class OpTrue : public OpCode {
public:
    OpTrue() : OpCode(OP_TRUE) {
    }
};

class OpFalse : public OpCode {
public:
    OpFalse() : OpCode(OP_FALSE) {
    }
};

class OpEqual : public OpCode {
public:
    OpEqual() : OpCode(OP_EQUAL) {
    }
};

class OpNotEqual : public OpCode {
public:
    OpNotEqual() : OpCode(OP_NOTEQUAL) {
    }
};

class OpGreaterThan : public OpCode {
public:
    OpGreaterThan() : OpCode(OP_GREATERTHAN) {
    }
};

class OpGreaterThanEqual : public OpCode {
public:
    OpGreaterThanEqual() : OpCode(OP_GREATERTHAN_EQUAL) {
    }
};

class OpMinus : public OpCode {
public:
    OpMinus() : OpCode(OP_MINUS) {
    }
};

class OpBang : public OpCode {
public:
    OpBang() : OpCode(OP_BANG) {
    }
};

class OpJumpNotTruthy : public OpCode {
public:
    OpJumpNotTruthy() : OpCode(OP_JUMP_NOT_TRUTHY) {
    }
};

class OpJump : public OpCode {
public:
    OpJump() : OpCode(OP_JUMP) {
    }
};

class OpNull : public OpCode {
public:
    OpNull() : OpCode(OP_NULL) {
    }
};

class OpGetGlobal : public OpCode {
public:
    OpGetGlobal() : OpCode(OP_GET_GLOBAL) {
    }
};

class OpSetGlobal : public OpCode {
public:
    OpSetGlobal() : OpCode(OP_SET_GLOBAL) {
    }
};

class OpArray : public OpCode {
public:
    OpArray() : OpCode(OP_ARRAY) {
    }
};

class OpHash : public OpCode {
public:
    OpHash() : OpCode(OP_HASH) {
    }
};

class OpIndex : public OpCode {
public:
    OpIndex() : OpCode(OP_INDEX) {
    }
};

class OpSetIndex : public OpCode {
public:
    OpSetIndex() : OpCode(OP_SET_INDEX) {
    }
};

class OpPostfixIndex : public OpCode {
public:
    OpPostfixIndex() : OpCode(OP_POSTFIX_INDEX) {
    }
};

class OpSlice : public OpCode {
public:
    OpSlice() : OpCode(OP_SLICE) {
    }
};

class OpPatchIndex : public OpCode {
public:
    OpPatchIndex() : OpCode(OP_PATCH_INDEX) {
    }
};

class OpCall : public OpCode {
public:
    OpCall() : OpCode(OP_CALL) {
    }
};

class OpCallKw : public OpCode {
public:
    OpCallKw() : OpCode(OP_CALL_KW) {
    }
};

class OpReturnValue : public OpCode {
public:
    OpReturnValue() : OpCode(OP_RETURN_VALUE) {
    }
};

class OpReturn : public OpCode {
public:
    OpReturn() : OpCode(OP_RETURN) {
    }
};

class OpGetLocal : public OpCode {
public:
    OpGetLocal() : OpCode(OP_GET_LOCAL) {
    }
};
    
class OpSetLocal : public OpCode {
public:
    OpSetLocal() : OpCode(OP_SET_LOCAL) {
    }
};

class OpGetBuiltin : public OpCode {
public:
    OpGetBuiltin() : OpCode(OP_GET_BUILTIN) {
    }
};

class OpClosure : public OpCode {
public:
    OpClosure() : OpCode(OP_CLOSURE) {
    }
};

class OpGetFree : public OpCode {
public:
    OpGetFree() : OpCode(OP_GET_FREE) {
    }
};

class OpCurrentClosure : public OpCode {
public:
    OpCurrentClosure() : OpCode(OP_CURRENT_CLOSURE) {
    }
};

class OpClass : public OpCode {
public:
    OpClass() : OpCode(OP_CLASS) {
    }
};

class OpSetMethod : public OpCode {
public:
    OpSetMethod() : OpCode(OP_SET_METHOD) {
    }
};

class OpInstantiate : public OpCode {
public:
    OpInstantiate() : OpCode(OP_INSTANTIATE) {
    }
};

class OpGetProperty : public OpCode {
public:
    OpGetProperty() : OpCode(OP_GET_PROPERTY) {
    }
};
    
class OpSetProperty : public OpCode {
public:
    OpSetProperty() : OpCode(OP_SET_PROPERTY) {
    }
};

class OpGetMethod : public OpCode {
public:
    OpGetMethod() : OpCode(OP_GET_METHOD) {
    }
};
    
class OpInvoke : public OpCode {
public:
    OpInvoke() : OpCode(OP_INVOKE) {
    }
};

class OpSetPropertySym : public OpCode {
public:
    OpSetPropertySym() : OpCode(OP_SET_PROPERTY_SYM) {
    }
};

class OpGetPropertySym : public OpCode {
public:
    OpGetPropertySym() : OpCode(OP_GET_PROPERTY_SYM) {
    }
};

class OpPatchPropertySym : public OpCode {
public:
    OpPatchPropertySym() : OpCode(OP_PATCH_PROPERTY_SYM) {
    }
};

class OpGetThis : public OpCode {
public:
    OpGetThis() : OpCode(OP_GET_THIS) {
    }
};

class OpGetSuperMethod : public OpCode {
public:
    OpGetSuperMethod() : OpCode(OP_GET_SUPER_METHOD) {
    }
};

class OpFileStmtOutput : public OpCode {
public:
    OpFileStmtOutput() : OpCode(OP_FILE_STMT_OUTPUT) {
    }
};

extern OpConstant opConstant;
extern OpAdd opAdd;
extern OpSub opSub;
extern OpMultiply opMultiply;
extern OpDivide opDivide;
extern OpPop opPop;
extern OpTrue opTrue;
extern OpFalse opFalse;
extern OpEqual opEqual;
extern OpNotEqual opNotEqual;
extern OpGreaterThan opGreaterThan;
extern OpGreaterThanEqual opGreaterThanEqual;
extern OpMinus opMinus;
extern OpBang opBang;
extern OpJumpNotTruthy opJumpNotTruthy;
extern OpJump opJump;
extern OpNull opNull;
extern OpGetGlobal opGetGlobal;
extern OpSetGlobal opSetGlobal;
extern OpArray opArray;
extern OpHash opHash;
extern OpIndex opIndex;
extern OpSetIndex opSetIndex;
extern OpPostfixIndex opPostfixIndex;
extern OpSlice opSlice;
extern OpPatchIndex opPatchIndex;
extern OpCall opCall;
extern OpCallKw opCallKw;
extern OpReturnValue opReturnValue;
extern OpReturn opReturn;
extern OpGetLocal opGetLocal;
extern OpSetLocal opSetLocal;
extern OpGetBuiltin opGetBuiltin;
extern OpClosure opClosure;
extern OpGetFree opGetFree;
extern OpCurrentClosure opCurrentClosure;
extern OpClass opClass;
extern OpSetMethod opSetMethod;
extern OpInstantiate opInstantiate;
extern OpGetProperty opGetProperty;
extern OpSetProperty opSetProperty;
extern OpGetMethod opGetMethod;
extern OpInvoke opInvoke;
extern OpSetPropertySym opSetPropertySym;
extern OpGetPropertySym opGetPropertySym;
extern OpPatchPropertySym opPatchPropertySym;
extern OpGetThis opGetThis;
extern OpGetSuperMethod opGetSuperMethod;
extern OpFileStmtOutput opFileStmtOutput;

#endif