# Plan: Stop method dispatch from exposing caller locals

## Problem

Assign semantics stay as defined:

- name exists on outer chain → update that binding
- else → create local

That correctly makes `examples/test190.ad` and `tests/parity/fixtures/functions/permute_n3_checksum.ad` work.

The bug is **who is on the outer chain** during method calls. Method dispatch wires the caller's frame onto `instance_environment->outer`, so `StringUtils.indexOf`'s `for (i = …)` sees the caller's `i` as an already-defined outer binding and mutates it. Same class of bug for `this.other()` when `ApplyMethod` encloses the calling method frame.

Field reads already avoid this by temporarily setting `outer = NULL`. Methods need globals, not the caller.

## Goal

Method / instance env chain should be:

```text
method frame → instance env → global env (→ bootstrap via Get)
```

Never:

```text
method frame → instance env → caller's locals
```

Do **not** change `Environment::Set` assign rules.

## Approach (evaluator-first)

1. Add `resolveGlobalEnvironment(Environment*)` — walk `outer` until `isGlobalEnvironment`.
2. `obj.method(...)`: set `klass_environment->outer = resolveGlobalEnvironment(&env)` (not `&env`); restore on all paths.
3. `this.method(...)` / `ExtendMethodEnv`: enclose the **instance environment**, not the calling method frame.
4. Constructor / field-init: prefer global via `resolveGlobalEnvironment` so constructors don't see caller locals.
5. Leave free functions (`ApplyFunction`) alone — they already enclose definition env (`test190`).

## Tests

- New regression: method call inside `for (i=…)` / local `i` must not be clobbered by `StringUtils.indexOf`.
- Keep green: `test190.ad`, `permute_n3_checksum.ad`, OOP examples, `test298.ad`.

## Out of scope

- VM `OP_SET_FREE`
- Renaming `ki`/`pi` workarounds in `bootstrap/net_utils.ad` (optional later)

## Success criteria

- Nested instance methods no longer corrupt caller locals like `i` / `result`
- Global mutation from free functions still works
- No permanent `instance.outer` left pointing at a caller frame after the call returns
