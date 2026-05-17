# Performance Tests (Evaluator vs VM)

This folder contains timing infrastructure and fixtures for comparing wall-clock execution of the tree-walking evaluator against the bytecode VM.

## Runner

- Script: `tests/performance/run_performance.py`
- Default binary: `./main`
- Default fixtures dir: `tests/performance/fixtures`

Run:

```bash
python3 tests/performance/run_performance.py
```

Optional flags (see `python3 tests/performance/run_performance.py --help`):

- `--warmup` — discarded runs per mode before timing (default: 1)
- `--iterations` — timed runs per mode per fixture (default: 10)

## Fixture format

- Use `.ad` source files.
- Keep fixtures deterministic and focused (one workload each).
- Prefer CPU-bound logic with minimal I/O so subprocess timing reflects interpreter work rather than console or disk noise.

## Notes

Results are **wall-clock** medians over repeated process runs; they include startup and teardown of each invocation, not in-process micro-benchmarking alone.

For stable comparisons on your machine, close heavy background load, fix CPU frequency if possible, and raise `--warmup` / `--iterations` when noise is high.
