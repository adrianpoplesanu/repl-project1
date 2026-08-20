#include "utils.h"
#include "code.h"
#include "opcode.h"
#include "objects.h"
#include <algorithm>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <set>
#include <sstream>

namespace {
std::string make_timestamp() {
    using namespace std::chrono;
    const auto now = system_clock::now();
    const auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;
    const std::time_t now_c = system_clock::to_time_t(now);
    std::tm tm_snapshot{};
#if defined(_WIN32)
    localtime_s(&tm_snapshot, &now_c);
#else
    localtime_r(&now_c, &tm_snapshot);
#endif
    std::ostringstream out;
    out << std::put_time(&tm_snapshot, "%Y%m%d-%H%M%S") << "-" << std::setw(3) << std::setfill('0') << ms.count();
    return out.str();
}

void append_instruction_block(
    std::ostream& out,
    const std::string& section_title,
    const Instructions* instructions
) {
    out << "=== " << section_title << " ===\n";
    if (instructions == nullptr) {
        out << "<missing instructions>\n\n";
        return;
    }
    out << "size: " << instructions->size << "\n";
    out << "bytes:";
    for (unsigned char byte : instructions->bytes) {
        out << " " << static_cast<int>(byte);
    }
    out << "\n";
    out << disassemble_instructions(*instructions) << "\n";
}

void append_object_instructions(
    std::ostream& out,
    Ad_Object* object,
    const std::string& path,
    std::set<const Ad_Object*>& visited
) {
    if (object == nullptr) {
        out << "=== " << path << " ===\n<null object>\n\n";
        return;
    }
    if (visited.find(object) != visited.end()) {
        out << "=== " << path << " ===\n<already logged>\n\n";
        return;
    }
    visited.insert(object);

    const Ad_Object_Type type = object->Type();
    if (type == OBJ_COMPILED_FUNCTION) {
        auto* fn = static_cast<AdCompiledFunction*>(object);
        std::ostringstream title;
        title << path << " [compiled_function] params=" << fn->num_parameters << " locals=" << fn->num_locals;
        append_instruction_block(out, title.str(), fn->instructions);
        return;
    }

    if (type == OBJ_CLOSURE) {
        auto* closure = static_cast<AdClosureObject*>(object);
        out << "=== " << path << " [closure] ===\n";
        out << "fn: " << closure->fn << "\n\n";
        append_object_instructions(out, closure->fn, path + ".fn", visited);
        return;
    }

    if (type == OBJ_COMPILED_CLASS) {
        auto* klass = static_cast<AdCompiledClass*>(object);
        out << "=== " << path << " [compiled_class] ===\n";
        out << "field_initializers: " << klass->field_initializers.size() << "\n";
        out << "methods: " << klass->methods.size() << "\n\n";

        for (size_t i = 0; i < klass->field_initializers.size(); ++i) {
            append_object_instructions(
                out,
                klass->field_initializers[i],
                path + ".field_initializer[" + std::to_string(i) + "]",
                visited
            );
        }
        for (const auto& entry : klass->methods) {
            append_object_instructions(
                out,
                entry.second,
                path + ".method[" + entry.first + "]",
                visited
            );
        }
        return;
    }
}
} // namespace

namespace {
// Static operand-width tables (2 = uint16, 1 = uint8). Zero-operand ops use nullptr.
static int ow_u16[] = {2};
static int ow_u8[] = {1};
static int ow_u8_u8[] = {1, 1};
static int ow_u16_u8[] = {2, 1};

static Definition kOpConstant("OpConstant", 1, ow_u16, false);
static Definition kOpAdd("OpAdd", 0, nullptr, false);
static Definition kOpSub("OpSub", 0, nullptr, false);
static Definition kOpMultiply("OpMultiply", 0, nullptr, false);
static Definition kOpDivide("OpDivide", 0, nullptr, false);
static Definition kOpMod("OpMod", 0, nullptr, false);
static Definition kOpPop("OpPop", 0, nullptr, false);
static Definition kOpTrue("OpTrue", 0, nullptr, false);
static Definition kOpFalse("OpFalse", 0, nullptr, false);
static Definition kOpEqual("OpEqual", 0, nullptr, false);
static Definition kOpNotEqual("OpNotEqual", 0, nullptr, false);
static Definition kOpGreaterThan("OpGreaterThan", 0, nullptr, false);
static Definition kOpGreaterThanEqual("OpGreaterThanEqual", 0, nullptr, false);
static Definition kOpAnd("OpAnd", 0, nullptr, false);
static Definition kOpOr("OpOr", 0, nullptr, false);
static Definition kOpMinus("OpMinus", 0, nullptr, false);
static Definition kOpBang("OpBang", 0, nullptr, false);
static Definition kOpJumpNotTruthy("OpJumpNotTruthy", 1, ow_u16, false);
static Definition kOpJump("OpJump", 1, ow_u16, false);
static Definition kOpNull("OpNull", 0, nullptr, false);
static Definition kOpGetGlobal("OpGetGlobal", 1, ow_u16, false);
static Definition kOpSetGlobal("OpSetGlobal", 1, ow_u16, false);
static Definition kOpArray("OpArray", 1, ow_u16, false);
static Definition kOpHash("OpHash", 1, ow_u16, false);
static Definition kOpIndex("OpIndex", 0, nullptr, false);
static Definition kOpSetIndex("OpSetIndex", 0, nullptr, false);
static Definition kOpPostfixIndex("OpPostfixIndex", 0, nullptr, false);
static Definition kOpSlice("OpSlice", 0, nullptr, false);
static Definition kOpPatchIndex("OpPatchIndex", 0, nullptr, false);
static Definition kOpCall("OpCall", 1, ow_u8, false);
static Definition kOpCallKw("OpCallKw", 2, ow_u8_u8, false);
static Definition kOpReturnValue("OpReturnValue", 0, nullptr, false);
static Definition kOpReturn("OpReturn", 0, nullptr, false);
static Definition kOpGetLocal("OpGetLocal", 1, ow_u8, false);
static Definition kOpSetLocal("OpSetLocal", 1, ow_u8, false);
static Definition kOpGetBuiltin("OpGetBuiltin", 1, ow_u8, false);
static Definition kOpClosure("OpClosure", 2, ow_u16_u8, false);
static Definition kOpGetFree("OpGetFree", 1, ow_u8, false);
static Definition kOpCurrentClosure("OpCurrentClosure", 0, nullptr, false);
static Definition kOpClass("OpClass", 0, nullptr, false);
static Definition kOpSetMethod("OpSetMethod", 0, nullptr, false);
static Definition kOpGetProperty("OpGetProperty", 0, nullptr, false);
static Definition kOpSetProperty("OpSetProperty", 0, nullptr, false);
static Definition kOpGetMethod("OpGetMethod", 0, nullptr, false);
static Definition kOpInvoke("OpInvoke", 1, ow_u8, false);
static Definition kOpSetPropertySym("OpSetPropertySym", 1, ow_u16, false);
static Definition kOpGetPropertySym("OpGetPropertySym", 1, ow_u16, false);
static Definition kOpPatchPropertySym("OpPatchPropertySym", 1, ow_u16, false);
static Definition kOpGetThis("OpGetThis", 0, nullptr, false);
static Definition kOpGetSuperMethod("OpGetSuperMethod", 0, nullptr, false);
static Definition kOpFileStmtOutput("OpFileStmtOutput", 0, nullptr, false);

struct DefEntry {
    unsigned char opcode;
    Definition* def;
};

static const DefEntry kDefEntries[] = {
    {OP_CONSTANT, &kOpConstant},
    {OP_ADD, &kOpAdd},
    {OP_SUB, &kOpSub},
    {OP_MULTIPLY, &kOpMultiply},
    {OP_DIVIDE, &kOpDivide},
    {OP_MOD, &kOpMod},
    {OP_POP, &kOpPop},
    {OP_TRUE, &kOpTrue},
    {OP_FALSE, &kOpFalse},
    {OP_EQUAL, &kOpEqual},
    {OP_NOTEQUAL, &kOpNotEqual},
    {OP_GREATERTHAN, &kOpGreaterThan},
    {OP_GREATERTHAN_EQUAL, &kOpGreaterThanEqual},
    {OP_AND, &kOpAnd},
    {OP_OR, &kOpOr},
    {OP_MINUS, &kOpMinus},
    {OP_BANG, &kOpBang},
    {OP_JUMP_NOT_TRUTHY, &kOpJumpNotTruthy},
    {OP_JUMP, &kOpJump},
    {OP_NULL, &kOpNull},
    {OP_GET_GLOBAL, &kOpGetGlobal},
    {OP_SET_GLOBAL, &kOpSetGlobal},
    {OP_ARRAY, &kOpArray},
    {OP_HASH, &kOpHash},
    {OP_INDEX, &kOpIndex},
    {OP_SET_INDEX, &kOpSetIndex},
    {OP_POSTFIX_INDEX, &kOpPostfixIndex},
    {OP_SLICE, &kOpSlice},
    {OP_PATCH_INDEX, &kOpPatchIndex},
    {OP_CALL, &kOpCall},
    {OP_CALL_KW, &kOpCallKw},
    {OP_RETURN_VALUE, &kOpReturnValue},
    {OP_RETURN, &kOpReturn},
    {OP_GET_LOCAL, &kOpGetLocal},
    {OP_SET_LOCAL, &kOpSetLocal},
    {OP_GET_BUILTIN, &kOpGetBuiltin},
    {OP_CLOSURE, &kOpClosure},
    {OP_GET_FREE, &kOpGetFree},
    {OP_CURRENT_CLOSURE, &kOpCurrentClosure},
    {OP_CLASS, &kOpClass},
    {OP_SET_METHOD, &kOpSetMethod},
    {OP_GET_PROPERTY, &kOpGetProperty},
    {OP_SET_PROPERTY, &kOpSetProperty},
    {OP_GET_METHOD, &kOpGetMethod},
    {OP_INVOKE, &kOpInvoke},
    {OP_SET_PROPERTY_SYM, &kOpSetPropertySym},
    {OP_GET_PROPERTY_SYM, &kOpGetPropertySym},
    {OP_PATCH_PROPERTY_SYM, &kOpPatchPropertySym},
    {OP_GET_THIS, &kOpGetThis},
    {OP_GET_SUPER_METHOD, &kOpGetSuperMethod},
    {OP_FILE_STMT_OUTPUT, &kOpFileStmtOutput},
};
} // namespace

// Static function to initialize and return the definitions map
static std::map<unsigned char, Definition*>& get_definitions_map() {
    static std::map<unsigned char, Definition*> definitions_map;
    static bool initialized = false;

    if (!initialized) {
        for (const DefEntry& entry : kDefEntries) {
            definitions_map[entry.opcode] = entry.def;
        }
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

void write_bytecode_log(const Bytecode& bytecode) {
    static std::mutex log_mutex;
    std::lock_guard<std::mutex> lock(log_mutex);

    const std::filesystem::path output_dir("bytecode-logs");
    std::error_code fs_error;
    std::filesystem::create_directories(output_dir, fs_error);
    if (fs_error) {
        std::cerr << "[ VM Log ] Failed to create bytecode-logs directory: " << fs_error.message() << std::endl;
        return;
    }

    const std::string timestamp = make_timestamp();
    const std::filesystem::path log_path = output_dir / ("bytecode-log-" + timestamp + ".log");
    std::ofstream log_file(log_path);
    if (!log_file.is_open()) {
        std::cerr << "[ VM Log ] Failed to create log file: " << log_path.string() << std::endl;
        return;
    }

    log_file << "timestamp: " << timestamp << "\n";
    log_file << "constants_count: " << bytecode.constants.size() << "\n\n";
    append_instruction_block(log_file, "main", &bytecode.instructions);

    std::set<const Ad_Object*> visited;
    for (size_t i = 0; i < bytecode.constants.size(); ++i) {
        Ad_Object* constant = bytecode.constants[i];
        std::string header = "constant[" + std::to_string(i) + "]";
        if (constant == nullptr) {
            log_file << "=== " << header << " ===\n<null>\n\n";
            continue;
        }
        log_file << "=== " << header << " type=" << constant->Type() << " ===\n";
        log_file << "inspect: " << constant->Inspect() << "\n\n";
        append_object_instructions(log_file, constant, header, visited);
    }
    log_file.close();

    std::vector<std::filesystem::directory_entry> log_files;
    for (const auto& entry : std::filesystem::directory_iterator(output_dir)) {
        if (!entry.is_regular_file()) {
            continue;
        }
        const std::string file_name = entry.path().filename().string();
        const bool has_prefix = file_name.rfind("bytecode-log-", 0) == 0;
        const bool has_suffix = file_name.size() >= 4 && file_name.substr(file_name.size() - 4) == ".log";
        if (has_prefix && has_suffix) {
            log_files.push_back(entry);
        }
    }
    std::sort(log_files.begin(), log_files.end(), [](const auto& left, const auto& right) {
        return left.last_write_time() < right.last_write_time();
    });
    while (log_files.size() > 10) {
        std::filesystem::remove(log_files.front().path(), fs_error);
        log_files.erase(log_files.begin());
    }
}

