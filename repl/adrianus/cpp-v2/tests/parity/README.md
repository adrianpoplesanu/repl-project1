# Parity Tests (Evaluator vs VM)

This folder contains differential parity infrastructure and fixtures.

## Runner

- Script: `tests/parity/run_parity.py`
- Default binary: `./main`
- Default fixtures dir: `tests/parity/fixtures`

Run:

```bash
python3 tests/parity/run_parity.py
```

## Fixture format

- Use `.ad` source files.
- Keep fixtures deterministic and focused (one feature cluster each).
- Prefer explicit output expressions or builtin prints.

## Notes

Current interpreter behavior differs between evaluator and VM in how file execution emits results.
The runner supports a future canonical output line format:

- `PARITY_RESULT:<normalized value>`

Once both paths emit this consistently for test mode, value comparison can be made strict.
