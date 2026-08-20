# Evaluator Performance Enhancement Plan

## Goal

Reduce wall-clock execution time and peak/steady memory for the tree-walk interpreter
(`evaluator.cpp`, `objects.*`, `environment.*`, `gc.*`) without changing Adrianus semantics.

## Current object / memory model (baseline)

### Declaration and representation

- All runtime values inherit `Ad_Object` (`objects.h`): `type`, unused `ref_count`,
  `marked`, intrusive `next`/`prev`, virtual methods (`Inspect`, `Type`, `Hash`, `copy`).
- Scalars (`OBJ_INT`, `OBJ_FLOAT`, `OBJ_BOOL`, `OBJ_STRING`, …) and containers
  (`OBJ_LIST`, `OBJ_HASH`, functions, instances, …) are all heap objects.
- Environments store `std::unordered_map<std::string, Ad_Object*>` with outer/sibling chains
  and a recursive mutex (`environment.cpp`).

### Allocation

- Hot path pattern: `new Ad_Integer_Object()` + `garbageCollector->addObject(obj)`.
- Literals, arithmetic, `++`/`--`, return wrappers, lists/hashes, call frames all allocate.
- Bools/null use permanent singletons (`TRUE` / `FALSE` / `NULLOBJECT`).

### Lifetime and freeing

- Mark-sweep over the GC linked list (`gc.cpp`).
- Roots: `mainEnv` + all `gc_environments` (+ siblings). Function envs rely on env refcounts.
- GC trigger: every `maxCycle2` (100) **top-level** statements in `EvalProgram`.
- `EvalWhileExpression` / `EvalForExpression` re-evaluate without mark/sweep → temps pile up
  for the life of the loop (comment at `evaluator.cpp` ~957).
- Shutdown: force-free paths in REPL/teardown.

### Lifecycle diagram

```
AST node → Eval() [ad_task_checkpoint]
  → new Ad_*_Object + gc.addObject
  → bind in Environment / List / Hash / return to caller
  → (every 100 top-level stmts) mark from envs → sweep unmarked → delete
Environments: INCREF on call → schedule DECREF → sweep when ref_count==0
```

---

## Diagnosis: what dominates cost

| Area | Symptom | Where |
|------|---------|--------|
| Boxed ints | Every `+`, `-`, compare, `++` allocates | `EvalIntegerInfixExpression`, postfix/prefix |
| Loop GC gap | Unbounded RSS in `while`/`for` | `EvalWhileExpression`, `EvalForExpression` |
| Hash keys | String build + hash + `to_string` per access | `Ad_*::Hash`, hash literal/index eval |
| Env lookup | Mutex + double map probe + string by value | `Environment::Get`/`Check`/`Set` |
| Dispatch | Giant switch + string operators + checkpoint every node | `Eval` |
| Call frames | New `Environment` + unordered_map per call | `extendFunctionEnv` / `ApplyFunction` |
| Assign copy | Extra int copies on identifier assign | assign path ~1164 |

---

## Recommendations — execution time

### P0 (highest leverage)

1. **Unbox small values**  
   Introduce a `Value` tagged union or tagged pointers for int / bool / null (and ideally float).  
   Keep heap objects only for strings, lists, hashes, functions, instances.  
   Fast-path integer infix without `new`.

2. **GC (or arena) inside loops**  
   Run mark/sweep on a budget inside while/for, or after N `Eval` calls (hook near
   `ad_task_checkpoint`). Alternatively bump-allocate expression temps and discard on
   statement boundary when not stored.

3. **Rewrite hash keying**  
   Stop using `object_type_map[type] + Inspect()` and `to_string(hash(...))` as map keys.  
   Prefer: interned string keys, integer keys, or a compact key type with proper equality.

4. **Cheap environment access**  
   - Pass `const std::string&` / `string_view`.  
   - Single walk with iterator (`find` once); remove Check-then-Get double lookup.  
   - Skip mutex on single-threaded eval path (or thread-local stores).

### P1

5. **Operator enum instead of `std::string` comparisons** in infix/prefix paths.
6. **In-place `++`/`--`** when the bound int is uniquely owned (or after unboxing, mutate locals).
7. **Pool / slot-based call frames** for small functions (fixed locals vector vs unordered_map).
8. **Use `obj->type` field** instead of virtual `Type()` on hot paths.
9. **Avoid unnecessary `copy()`** on int assign-from-identifier once ints are immutable/tagged.

### P2

10. Special-case tight numeric loops with a mini bytecode or jump to VM for hot functions.
11. Reduce `ad_task_checkpoint` frequency (every N nodes, not every node) if preemptibility allows.
12. Arena for short-lived error/return wrappers if those stay boxed.

---

## Recommendations — memory consumption

### P0

1. **Collect during loops** (same as time P0 #2) — fixes peak RSS for long `while`.
2. **Nursery / bump allocator** for expression temps; promote only when stored in env/list/hash.
3. **Trigger GC on allocation count or bytes**, not only top-level statement count.

### P1

4. Small-int intern pool (−128…256) if full unboxing is deferred.
5. Share list/hash element pointers under GC; deep-copy only on true clone semantics.
6. Deduplicate hash storage (avoid key string in map + again inside `HashPair`).
7. Ensure all allocation sites call `addObject` (audit `eval_utils.cpp` member helpers).

### P2

8. Generational GC (nursery → old) once roots/stack discipline are solid.
9. Compact permanent constants; never put immortal singletons on the sweep list.

---

## Phased delivery plan

| Phase | Work | Success metric |
|-------|------|----------------|
| E1 | Loop GC + alloc-count trigger | Stable RSS on `while` microbench |
| E2 | Hash key rewrite | Hash-heavy benches faster + less alloc |
| E3 | Env lookup / mutex / string_view | Identifier-heavy benches faster |
| E4 | Unbox int/bool/null | Arithmetic loop time ↓ substantially |
| E5 | Frame pooling + operator enums | Call-heavy / infix benches |
| E6 | Nursery/arena | Lower peak RSS, fewer full sweeps |

## Measurement

- Microbenches: tight `while` counter; hash get/set; recursive fib; string concat.
- Track: wall time, `getTotalResidualGCObjects()`, peak RSS, allocs/op (instrument `addObject`).
- Compare against current evaluator and VM on the same `.ad` examples.

## Risks / constraints

- Unboxing changes identity/`Inspect`/`Hash` semantics for ints — keep language tests green.
- Thread/task features depend on checkpoint + env mutex; don’t break concurrency.
- Prefer shared `Value` design with the VM path long-term to avoid two object models forever.

## Key files

| Path | Role |
|------|------|
| `objects.h` / `objects.cpp` | Type hierarchy, Hash/copy, free |
| `gc.h` / `gc.cpp` | Mark-sweep + env scheduling |
| `environment.h` / `environment.cpp` | Scope chains, mutexed store |
| `evaluator.h` / `evaluator.cpp` | Tree-walk Eval |
| `eval_utils.cpp` | Import + method helpers |
| `repl.cpp` | Wires GC, env, teardown free |
| `hashpair.h` | Hash entry key/value pointers |
| `ast.h` / `ast.cpp` | AST + AST refcounting |
