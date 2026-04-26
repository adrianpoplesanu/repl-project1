# VM Parity Implementation Plan

## Context

This project currently has two execution paths:

- `Evaluator` (AST interpreter) with broad language/runtime coverage.
- `VM` (`Compiler` + bytecode + `VM::run`) with strong foundations but narrower coverage.

The objective is to make VM behavior match evaluator behavior for the same language programs.

## Scope and Principles

### Primary goal

Achieve functional parity so existing evaluator-oriented scripts run under VM with equivalent:

- Returned values
- Side effects
- Error behavior
- Control-flow behavior

### Non-goals (initially)

- Perfect internal architecture parity (VM can differ internally).
- Premature micro-optimizations before parity confidence.

### Parity philosophy

- Match evaluator semantics first.
- Improve VM design second.
- Optimize third.

## Baseline Assessment

### Evaluator strengths (already implemented)

Evaluator supports a broad set of constructs including:

- `if`, `while`, `for`, `break`, `continue`
- class-related behavior (`class`, member access, methods, inheritance-related updates)
- assignment variants (`=`, `+=`)
- `this`/object-oriented method dispatch
- list/string indexing and slicing variants
- runtime-integrated objects (file/socket/thread methods)
- task-aware behavior (`spawn`/`await` style flow through scheduler APIs)

### VM and compiler strengths (already implemented)

Compiler + VM already support:

- constants, booleans, null, arithmetic, comparisons
- globals/locals
- arrays, hashes, index operations
- conditionals and jumps
- function literals, closures, function calls, returns
- basic class call scaffolding and some property/method opcodes in opcode set

### High-level gaps

1. AST nodes supported by evaluator but missing in compiler emission.
2. Opcodes defined but not fully executed in `VM::run`.
3. Semantic edge cases (truthiness, slicing, assignment, errors) not fully aligned.
4. Runtime integration (scheduler/thread/socket/file) not fully available in VM path.

## Delivery Strategy

Use a test-first parity loop:

1. Add parity tests for one feature cluster.
2. Implement compiler + VM support for that cluster.
3. Run differential tests vs evaluator.
4. Move to next cluster only after green tests.

This limits regressions and keeps progress measurable.

## Phase-by-Phase Plan

## Phase 0 - Project setup and parity matrix (2-3 days)

### Tasks

- Create a feature matrix keyed by AST statement/expression type (`ST_*`).
- Mark each feature with:
  - evaluator support
  - compiler support
  - VM opcode/runtime support
  - parity test coverage status
- Decide explicit parity contract:
  - value equality rules
  - error equivalence rules
  - side-effect equivalence boundaries

### Deliverables

- `docs/vm-parity-matrix.md`
- `docs/vm-parity-contract.md`

### Exit criteria

- Team can answer "is feature X parity-ready?" from one matrix row.

## Phase 1 - Differential test harness (3-5 days)

### Tasks

- Build a runner that executes the same source snippet with:
  - evaluator pipeline
  - VM pipeline
- Normalize outputs into a comparable representation:
  - scalar values
  - container values
  - object identity-sensitive outputs (where needed)
  - error type/message normalization
- Add fixture folders by feature cluster.

### Suggested fixture structure

- `tests/parity/core/`
- `tests/parity/functions/`
- `tests/parity/control_flow/`
- `tests/parity/oop/`
- `tests/parity/runtime/`
- `tests/parity/async/`

### Deliverables

- Failing parity suite showing current deltas.

### Exit criteria

- Any new implementation change can be validated by parity diff.

## Phase 2 - Core language parity (1-2 weeks)

### Focus

Close gaps for language basics before advanced runtime.

### Tasks

- Add compile/run support for missing core nodes:
  - `ST_FLOAT`
  - `ST_WHILE_EXPRESSION`
  - `ST_FOR_EXPRESSION`
  - `ST_BREAK_STATEMENT`
  - `ST_CONTINUE_STATEMENT`
  - `ST_PLUS_EQUALS`
  - `ST_MINUS_EQUALS` (if required by parser and evaluator behavior)
- Implement reliable jump patching for loops and branch targets.
- Ensure stack/local/global mutation semantics match evaluator.

### Parity checks

- nested loops with `break`/`continue`
- arithmetic with int/float combinations
- assignment/update inside and outside function scope

### Exit criteria

- Core/control-flow parity tests green.

## Phase 3 - Functions and closures hardening (4-6 days)

### Focus

Stabilize callable behavior before object model complexity.

### Tasks

- Validate parity of:
  - recursion
  - nested closures
  - free variable capture
  - return propagation through nested blocks
- Ensure argument count mismatch behavior aligns with evaluator.
- Confirm local slot handling and stack pointer rules under recursion.

### Exit criteria

- No parity regressions in function/closure fixture suite.

## Phase 4 - OOP parity (1-2 weeks)

### Focus

Bridge class/member/method semantics.

### Tasks

- Compiler emission for:
  - `ST_CLASS_STATEMENT`
  - `ST_MEMBER_ACCESS`
  - `ST_THIS_EXPRESSION`
  - `ST_SUPER_EXPRESSION` (if evaluator semantics are stable/expected)
- VM execution wiring for object opcodes:
  - property get/set paths
  - method lookup/binding
  - constructor invocation and initialization ordering
  - inheritance method resolution path
- Validate assignment to members and chained member access.

### Parity checks

- constructor field initialization order
- method overriding
- `this` binding in nested calls
- inherited method dispatch

### Exit criteria

- OOP fixture suite green with documented known limitations (if any).

## Phase 5 - Collections and slicing semantics (3-5 days)

### Focus

Align evaluator behavior for index/slice edge cases.

### Tasks

- Match list/string slicing semantics:
  - omitted start/end/step
  - negative indexes
  - out-of-range behavior
  - empty-range behavior
- Align hash key handling and error/null behavior for missing keys.

### Exit criteria

- Slice/index fixtures produce identical normalized outputs.

## Phase 6 - Runtime object parity (4-7 days)

### Focus

Bring evaluator-backed runtime objects to VM path.

### Tasks

- Decide integration model:
  - VM-native builtins
  - adapter reusing evaluator runtime handlers
- Add support for runtime object methods used today:
  - file methods
  - socket methods
  - thread methods
- Define thread-safe behavior boundaries for VM calls.

### Exit criteria

- Runtime fixture suite green for agreed method subset.

## Phase 7 - Async/task parity (4-7 days)

### Focus

Add `spawn`/`await` semantics to VM pipeline in a way compatible with scheduler.

### Tasks

- Compile support for:
  - `ST_SPAWN_EXPRESSION`
  - `ST_AWAIT_EXPRESSION`
- VM execution model for pending/completed task values.
- Integrate with `TaskScheduler` lifecycle and error propagation.
- Clarify cancellation/timeouts semantics (if relevant).

### Exit criteria

- Async fixtures green with deterministic behavior in tests.

## Phase 8 - Hardening, performance, and release (3-5 days)

### Tasks

- Run extended parity corpus and stress tests.
- Investigate memory ownership and GC interaction under VM-heavy paths.
- Benchmark evaluator vs VM on representative workloads.
- Add regression guardrails in CI.

### Exit criteria

- Stable parity baseline and documented performance profile.

## Work Breakdown by Code Area

### `vm/compiler.cpp`

- Add missing AST case handlers.
- Improve jump bookkeeping for loops and control statements.
- Ensure symbol resolution rules align with evaluator semantics.

### `vm/vm.cpp`

- Expand opcode execution loop for full opcode set used by compiler.
- Ensure stack/frame correctness under mixed call styles (closure/builtin/method/class).
- Align error handling behavior with evaluator where user-visible.

### `vm/opcode.*` and `vm/code.*`

- Add/validate opcode definitions and operand widths as needed.
- Keep encoding/decoding contract explicit and test-covered.

### Test layers

- Compiler unit tests for emitted instruction sequences.
- VM unit tests for opcode semantics.
- End-to-end parity tests evaluator vs VM (source-level).

## Risk Register and Mitigations

### Risk: semantic drift in edge cases

- **Mitigation:** differential tests for every new feature cluster before merge.

### Risk: class/inheritance complexity explodes scope

- **Mitigation:** split OOP parity into milestones:
  - class + fields
  - method dispatch
  - inheritance/super

### Risk: async behavior nondeterminism in tests

- **Mitigation:** deterministic scheduler mode and timeout-safe harness hooks.

### Risk: stack/frame bugs cause intermittent failures

- **Mitigation:** add invariant checks (debug builds) around `sp`, frame base pointers, and call boundaries.

## Suggested Timeline (single engineer)

- Phase 0-1: week 1
- Phase 2-3: week 2-3
- Phase 4: week 4-5
- Phase 5-6: week 6
- Phase 7-8: week 7-8

Total: approximately 6-8 weeks for high-confidence parity, or 4-5 weeks for core parity excluding advanced runtime/async.

## Milestones and Definition of Done

### Milestone A - Core parity

- Core/control/function tests green.
- Known non-core gaps documented.

### Milestone B - OOP parity

- Class/member/method tests green.
- Constructor/dispatch semantics locked.

### Milestone C - Runtime + async parity

- Runtime object + async suites green.
- Scheduler integration validated.

### Final DoD

- Parity matrix rows closed for targeted feature set.
- Differential suite green in CI.
- No open P1 semantic mismatches.
- Documentation updated with any intentional deviations.

## Recommended First Sprint Backlog

1. Build parity matrix and contract docs.
2. Implement evaluator-vs-VM differential test harness.
3. Add fixtures for core/control/function clusters.
4. Implement compiler+VM support for `while`, `break`, `continue`.
5. Implement/align float behavior.
6. Stabilize with regression tests before moving to OOP.
