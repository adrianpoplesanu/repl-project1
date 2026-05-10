# Phase 3 — Functions and closures (VM / evaluator parity)

## Goal

Stabilize callable behavior so VM programs match the evaluator for recursion, lexical captures, returns, and basic arity rules—before investing in OOP (Phase 4).

## Scope (from `vm-evaluator-parity-plan.md`)

- Recursion through `def` / function values bound in globals or locals.
- Nested functions with **free variables** (outer locals or outer captures).
- `return` through normal control flow (including branches); VM already compiles `OP_RETURN_VALUE` / `OP_RETURN`.
- **Too few arguments** on user `def` / function calls: align with `Evaluator::ApplyFunction` (same error message and printed form).
- **Extra arguments**: evaluator still calls the function and binds only the leading parameters; VM allows `num_args >= num_parameters` and uses the same frame layout.

## Design

### Closure representation

- `AdClosureObject` holds `fn` (`AdCompiledFunction*`) and `free_vars` (`std::vector<Ad_Object*>`), mirroring captured bindings in the evaluator’s extended environments.
- `OP_CLOSURE constIdx numFree` pops `numFree` values (stack order reversed when filling `free_vars` so indices match the compiler’s `free_symbols` order).
- `OP_GET_FREE index` loads `current_frame()->cl->free_vars[index]`.
- `OP_CURRENT_CLOSURE` pushes the closure of the active frame (self / recursion by name).

### Compiler (existing behavior, now honored by VM)

- For each `free_symbols` entry, the compiler emits `load_symbol` **before** `OP_CLOSURE` so captured values are on the stack when the closure is built.
- `SymbolTable::resolve` / `define_free` already classify outer locals and outer frees; emitted `OP_GET_LOCAL` / `OP_GET_GLOBAL` / `OP_GET_FREE` at **creation** sites run in the enclosing function’s frame.

### Arity (too few)

- When `num_args < num_parameters`, VM does **not** enter a new frame: it removes callee and arguments from the stack, pushes `new Ad_Error_Object("some error message here")` (same string as the evaluator path), optionally registers it on `gc`.

## Parity fixtures

Under `tests/parity/fixtures/functions/`:

| Fixture | Intent |
|--------|--------|
| `recursion_factorial.ad` | Classic recursion |
| `closure_make_adder.ad` | Higher-order: returned inner function captures outer parameter |
| `return_nested_if.ad` | `return` from inside `if` block inside `def` |
| `wrong_arity_too_few.ad` | Too few positional args → same `ERROR: …` line as evaluator |

Run with the existing harness:

```bash
python3 tests/parity/run_parity.py
```

## Exit criteria

- All Phase 3 fixtures pass evaluator vs VM (`run_parity.py`).
- No regressions in existing `tests/parity/fixtures/core` and `control_flow` suites.

## Follow-ups (not in Phase 3)

- Default parameters and keyword arguments parity (evaluator has richer `ApplyFunction` logic).
- `OP_SET_FREE` / assignment to captured variables if the language guarantees mutating outer bindings.
- GC tracing of `free_vars` (collector’s `markObjects` is still a stub in this tree).
