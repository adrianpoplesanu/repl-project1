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

