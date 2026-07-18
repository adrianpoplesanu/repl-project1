# VM Performance Enhancement Plan

## Goal

Reduce execution time and memory for the bytecode VM (`vm/vm.cpp`, `vm/compiler.cpp`,
shared `objects.*` / `gc.*`) while preserving evaluator parity where required.

## Current object / memory model (baseline)

### Declaration and representation

- Same boxed `Ad_Object*` model as the evaluator; stack/globals/constants hold pointers.
- VM types: `OBJ_COMPILED_FUNCTION`, `OBJ_CLOSURE`, `OBJ_COMPILED_CLASS`,
  `OBJ_COMPILED_INSTANCE`, bound methods (`vm/objects.h`).
- No tagged pointers / NaN-boxing; `vm2/value.h` is currently empty scaffolding.
- Operand stack: fixed `Ad_Object* stack[2048]`; frames hold closure + IP + base pointer.

### Allocation

- Arithmetic: `new Ad_Integer_Object` / float / string + `vm_track` → `gc->addObject`.
- Compiler constants also `addObject`’d into the GC list.
- Closures, instances, bound methods allocated on call/construct paths.

### Lifetime and freeing

- **Critical gap:** VM never calls `markObjects` / `sweepObjects` during `run`.
- Objects stay on the GC list for the entire program; freed only via
  `garbageCollector->forceFreeObjects()` after run (`repl.cpp`).
- Unused API: `GarbageCollector::markObjects(Ad_Object** stack, int sp)` exists but is unused.
- Default `markObjects()` only walks environments — insufficient for VM roots
  (stack, globals, constants, frames, closures).

### Lifecycle diagram

```
AST → Compiler (constants → GC list) → Bytecode
  → VM::load (main closure + frame)
  → run / execute_instruction
       OP_CONSTANT: push borrowed ptr
       OP_ADD/…: new boxed result + addObject
       OP_CALL/RETURN: frames on stack slots
  → forceFreeObjects() (unmark all → sweep all → delete)
```

---

## Diagnosis: what dominates cost

| Area | Symptom | Where |
|------|---------|--------|
| No mid-run GC | Memory grows with every arithmetic op | `vm.cpp` binary ops; `repl.cpp` teardown-only free |
| Boxed arithmetic | `new` + list insert per op | `execute_binary_operation` ~1253+ |
| Dispatch | Long if/else chain | `execute_instruction` |
| Hash keys | Same string-of-hash scheme | `build_hash`, `execute_hash_index` |
| Class init | Full lower-stack preserve/copy | `call_class` |
| List slice | Deep `copy()` of every element | `vm_new_sub_list` |
| Logging | Filesystem bytecode dump every run | `vm.printLogs()` in `repl.cpp` |
| Virtual Type() | Extra indirection | binary/compare paths |
| Opcode lookup | `std::map` for definitions | `vm/utils.cpp` |

---

## Recommendations — execution time

### P0

1. **Unbox integers (then floats/bools)** on the VM stack  
   `Value` union or tagged pointer; specialize `OP_ADD`/`OP_SUB`/`OP_MUL`/`OP_DIV`/compare.  
   Largest win for numeric loops (e.g. `examples/test296.ad`-style workloads).

2. **Wire mid-run GC with correct VM roots**  
   Periodically: mark stack `[0, sp)`, globals, constants, frames’ closures/`bound_instance`,
   then `sweepObjects`. Reuse evaluator’s `cycle2`/`maxCycle2` pattern or alloc/byte threshold.  
   Do **not** call `forceFreeObjects` mid-run (it unmarks everything then frees all).

3. **Faster dispatch**  
   Dense `switch` or computed goto; cache `bytes` pointer + length in the current frame.

4. **Fix hash representation** (same as evaluator) — eliminate `to_string(hash(Hash()))`.

### P1

5. Use `obj->type` instead of virtual `Type()` in the interpreter loop.
6. Specialize `call_class` — avoid O(stack) preserve; mirror `call_closure` stack discipline.
7. Gate `printLogs()` behind a debug flag / env var (default off).
8. Opcode table as `Definition*[256]` instead of `std::map`.
9. Share pointers on list slice under GC instead of deep-copying elements.

### P2

10. Avoid allocating a fresh `Ad_Integer_Object(1)` constant per `++` site when possible — intern.
11. Cache resolved global indices; trim undefined-global fallback cost.
12. Consider threaded interpreter / superinstructions for common pairs (`GET_LOCAL`+`ADD`).

---

## Recommendations — memory consumption

### P0

1. **Periodic GC during `run`** with full VM root set (above) — stops unbounded growth.
2. **Unboxing** removes the dominant source of ephemeral heap objects.

### P1

3. Mark constants / permanent singletons as immortal (skip linking into sweep list, or never unmark).
4. Nursery bump region for remaining short-lived heap objects if unboxing is incomplete.
5. Share slice elements; deep-copy only on mutating operations that require isolation.
6. Compact `Instructions` (`bytes` + redundant `size` if size == vector length).

### P2

7. Generational GC once roots are correct.
8. Align / share object model with evaluator `Value` design to one allocator and one GC.
9. Revisit `vm2/` as experimental Value/chunk playground only after production VM has unboxing + GC.

---

## Phased delivery plan

| Phase | Work | Success metric |
|-------|------|----------------|
| V1 | Mid-run GC + VM roots (stack/globals/constants/frames) | Stable RSS on long loops |
| V2 | Disable default `printLogs` | Faster short scripts / less I/O |
| V3 | Hash key rewrite | Hash benches + less string alloc |
| V4 | Dispatch + `type` field + opcode table | Lower ns/instruction |
| V5 | Unbox int/bool on stack | Numeric loops approach C-like overhead |
| V6 | call_class / slice / constant interning | Lower allocs on OOP + list code |
| V7 | Shared Value model with evaluator | One memory story for both runtimes |

## Measurement

- Same microbenches as evaluator; also instruction count vs time (`run_vm_with_optional_instruction_limit`).
- Instrument: allocations via `addObject`, residual GC objects mid-run (must stay bounded), peak RSS.
- Regression: VM/evaluator parity suite (`docs/vm-parity-*`).

## Risks / constraints

- Enabling GC without marking stack/globals will free live objects → crashes/corruption.
- `forceFreeObjects` must remain shutdown-only.
- Closures with `owns_fn` and bound methods need careful mark edges.
- Threads with per-thread GC (`internal_gc`) need the same rooting rules.
- Unboxing must preserve `==`, hashing, and `Inspect` behavior expected by parity tests.

## Key files

| Path | Role |
|------|------|
| `objects.h` / `objects.cpp` | Shared boxed object hierarchy + free |
| `gc.h` / `gc.cpp` | Mark-sweep; unused stack-root overload |
| `vm/vm.cpp` / `vm/vm.h` | Dispatch, stack, arithmetic, calls |
| `vm/objects.h` / `vm/objects.cpp` | Closures, compiled classes/instances |
| `vm/compiler.cpp` | Constant pool + bytecode emit |
| `vm/frame.h` | Call frames |
| `vm/utils.cpp` | Opcode lookup, logging |
| `repl.cpp` | Load/run + `forceFreeObjects` teardown |
| `vm2/` | Experimental Value scaffolding (empty today) |
