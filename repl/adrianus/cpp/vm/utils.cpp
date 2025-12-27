#include "utils.h"
#include "code.h"
#include "opcode.h"
#include <iostream>
#include <sstream>

// Static function to initialize and return the definitions map
static std::map<unsigned char, Definition*>& get_definitions_map() {
    static std::map<unsigned char, Definition*> definitions_map;
    static bool initialized = false;
    
    if (!initialized) {
        definitions_map[OP_CONSTANT] = new Definition("OpConstant", 1, new int[1] {2});
        definitions_map[OP_ADD] = new Definition("OpAdd", 0, new int);
        definitions_map[OP_SUB] = new Definition("OpSub", 0, new int);
        definitions_map[OP_MULTIPLY] = new Definition("OpMultiply", 0, new int);
        definitions_map[OP_DIVIDE] = new Definition("OpDivide", 0, new int);
        definitions_map[OP_POP] = new Definition("OpPop", 0, new int);
        definitions_map[OP_TRUE] = new Definition("OpTrue", 0, new int);
        definitions_map[OP_FALSE] = new Definition("OpFalse", 0, new int);
        definitions_map[OP_EQUAL] = new Definition("OpEqual", 0, new int);
        definitions_map[OP_NOTEQUAL] = new Definition("OpNotEqual", 0, new int);
        definitions_map[OP_GREATERTHAN] = new Definition("OpGreaterThan", 0, new int);
        definitions_map[OP_GREATERTHAN_EQUAL] = new Definition("OpGreaterThanEqual", 0, new int);
        definitions_map[OP_MINUS] = new Definition("OpMinus", 0, new int);
        definitions_map[OP_BANG] = new Definition("OpBang", 0, new int);
        definitions_map[OP_JUMP_NOT_TRUTHY] = new Definition("OpJumpNotTruthy", 1, new int[1] {2});
        definitions_map[OP_JUMP] = new Definition("OpJump", 1, new int[1] {2});
        definitions_map[OP_NULL] = new Definition("OpNull", 0, new int);
        definitions_map[OP_GET_GLOBAL] = new Definition("OpGetGlobal", 1, new int[1] {2});
        definitions_map[OP_SET_GLOBAL] = new Definition("OpSetGlobal", 1, new int[1] {2});
        definitions_map[OP_ARRAY] = new Definition("OpArray", 1, new int[1] {2});
        definitions_map[OP_HASH] = new Definition("OpHash", 1, new int[1] {2});
        definitions_map[OP_INDEX] = new Definition("OpIndex", 0, new int);
        definitions_map[OP_CALL] = new Definition("OpCall", 1, new int[1] {1});
        definitions_map[OP_RETURN_VALUE] = new Definition("OpReturnValue", 0, new int);
        definitions_map[OP_RETURN] = new Definition("OpReturn", 0, new int);
        definitions_map[OP_GET_LOCAL] = new Definition("OpGetLocal", 1, new int[1] {1});
        definitions_map[OP_SET_LOCAL] = new Definition("OpSetLocal", 1, new int[1] {1});
        definitions_map[OP_GET_BUILTIN] = new Definition("OpGetBuiltin", 1, new int[1] {1});
        definitions_map[OP_CLOSURE] = new Definition("OpClosure", 2, new int[2] {2, 1});
        definitions_map[OP_GET_FREE] = new Definition("OpGetFree", 1, new int[1] {1});
        definitions_map[OP_CURRENT_CLOSURE] = new Definition("OpCurrentClosure", 0, new int);
        definitions_map[OP_CLASS] = new Definition("OpClass", 0, new int);
        definitions_map[OP_SET_METHOD] = new Definition("OpSetMethod", 0, new int);
        definitions_map[OP_GET_PROPERTY] = new Definition("OpGetProperty", 0, new int);
        definitions_map[OP_SET_PROPERTY] = new Definition("OpSetProperty", 0, new int);
        definitions_map[OP_SET_PROPERTY_SYM] = new Definition("OpSetPropertySym", 1, new int[1] {2});
        definitions_map[OP_GET_PROPERTY_SYM] = new Definition("OpGetPropertySym", 1, new int[1] {2});
        definitions_map[OP_PATCH_PROPERTY_SYM] = new Definition("OpPatchPropertySym", 1, new int[1] {2});
        initialized = true;
    }
    
    return definitions_map;
}

Definition* lookup(unsigned char _byte) {
    std::map<unsigned char, Definition*>& definitions_map = get_definitions_map();
    auto it = definitions_map.find(_byte);
    if (it != definitions_map.end()) {
        return it->second;
    }
    return nullptr;
}

std::string disassemble_instructions(const Instructions& instructions) {
    std::string out;
    int i = 0;
    
    while (i < instructions.size) {
        Definition* definition = lookup(instructions.bytes[i]);
        if (definition == nullptr) {
            std::ostringstream error;
            error << "ERROR: unknown opcode " << static_cast<int>(instructions.bytes[i]) << " at offset " << i;
            out += error.str();
            break;
        }
        
        std::vector<int> operands;
        int read = read_operands(*definition, instructions, i + 1, operands);
        out += format_int(i) + " " + format_instruction(*definition, operands) + '\n';
        i += 1 + read;
    }
    
    return out;
}

