# VM/Evaluator Parity Contract

## Purpose

Define how we decide whether `Evaluator` and `VM` behave the same for a given source program.

## Parity Levels

- **L0 Syntax parity**: both pipelines parse/compile without internal crashes.
- **L1 Execution parity**: both pipelines terminate with equivalent success/failure status.
- **L2 Value parity**: both pipelines produce equivalent user-visible result values.
- **L3 Behavior parity**: side effects and runtime semantics match (I/O, object mutation, async state).

Milestone A targets L0-L2 for core language constructs.
Milestone B/C extend L2 and L3 for OOP/runtime/async features.

## Comparison Rules

## 1. Status comparison

For each fixture run:

- compare process exit code
- compare whether stderr contains VM/Evaluator internal fatal markers

Mismatch at this level is a hard failure.

## 2. Result comparison

Preferred approach:

- both engines emit one canonical line prefixed with `PARITY_RESULT:`
- compare canonical payload after normalization

Interim approach (before full adapter):

- compare last non-empty, non-debug output line from each engine
- ignore known banner/debug lines

## 3. Error comparison

When execution fails by design:

- compare normalized error category first (e.g. `type`, `name`, `arity`, `runtime`)
- compare message text second (best effort, not strict byte-for-byte in early phase)

## 4. Side effects comparison (later phases)

For fixtures that interact with runtime objects:

- compare explicit textual traces emitted by fixture script
- avoid nondeterministic wall-clock text in assertions

## Normalization Rules

The differential runner may normalize outputs by:

- trimming trailing whitespace
- removing known VM-only debug lines (e.g. bytecode logs)
- removing evaluator/VM banners
- collapsing repeated blank lines

Normalization must not alter semantic payload lines.

## Fixture Authoring Rules

- one feature focus per fixture file
- deterministic behavior only
- no external network reliance
- avoid random/time-sensitive values unless explicitly mocked
- include expected classification metadata where needed

## Severity Policy

- **P0**: crash/hang/memory corruption
- **P1**: user-visible semantic mismatch (wrong value or control-flow)
- **P2**: error text/detail mismatch with same semantic category
- **P3**: non-semantic formatting/logging mismatch

## Acceptance Gates

Phase gate can be considered complete only when:

- all fixtures in that phase are green in differential runner
- all new features added in phase have at least one positive and one edge-case fixture
- no open P0/P1 issues for covered scope
