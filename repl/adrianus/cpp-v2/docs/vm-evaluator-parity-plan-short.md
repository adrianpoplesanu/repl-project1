# VM Parity Plan (Short)

## Objective

Reach practical feature parity between `Evaluator` and `VM` so existing language programs behave the same on both execution paths.

## Target Milestones

## Milestone A - Core Parity

Scope:

- literals, arithmetic, booleans, null
- `if`, loops, `break`, `continue`
- `let`, assignment variants, globals/locals
- function calls, closures, returns
- list/hash/index baseline behavior

Done when:

- Core parity test suite is green.
- No open P1 mismatches in core semantics.

## Milestone B - OOP Parity

Scope:

- class definitions
- member get/set
- method calls and constructor behavior
- `this` (and `super` if supported in evaluator path)

Done when:

- OOP parity suite is green.
- Method binding/dispatch matches evaluator in fixture corpus.

## Milestone C - Runtime + Async Parity

Scope:

- runtime object methods (file/socket/thread used by current scripts)
- `spawn`/`await` behavior integrated with scheduler

Done when:

- Runtime and async parity suites are green.
- Deterministic tests pass in CI.

## Work Phases (Checklist)

- [ ] Phase 0: Feature matrix and parity contract docs
- [ ] Phase 1: Differential test harness (Evaluator vs VM)
- [ ] Phase 2: Core compiler/VM gaps (`while/for/break/continue`, float, assignment updates)
- [ ] Phase 3: Function/closure hardening
- [ ] Phase 4: OOP parity
- [ ] Phase 5: Slicing/index edge-case parity
- [ ] Phase 6: Runtime object parity
- [ ] Phase 7: Async/task parity (`spawn`/`await`)
- [ ] Phase 8: Stabilization, performance, CI hardening

## Suggested Timeline

- Week 1: phases 0-1
- Week 2-3: phases 2-3
- Week 4-5: phase 4
- Week 6: phases 5-6
- Week 7-8: phases 7-8

Fast-track (core-first): 4-5 weeks  
Full parity (including runtime/async): 6-8 weeks

## Top Risks

- semantic drift in edge cases (truthiness/slicing/errors)
- OOP dispatch and inheritance corner cases
- async nondeterminism in tests
- stack/frame correctness under recursion and mixed call styles

## Risk Controls

- differential parity tests for every feature cluster
- milestone gates (no jump to next milestone with red parity tests)
- deterministic scheduler mode for async tests
- debug invariants for stack/frame boundaries

## Definition of Done (Project)

- parity matrix closed for agreed scope
- differential suite green in CI
- no unresolved P1 behavioral mismatches
- docs updated with intentional deviations (if any)
