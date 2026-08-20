#!/usr/bin/env python3
"""
Performance harness: wall-clock execution time for Evaluator vs VM.

Modeled after tests/parity/run_parity.py (same binary / subprocess contract),
but measures repeated runs with optional warmup instead of comparing stdout.
"""

from __future__ import annotations

import argparse
import dataclasses
import os
import pathlib
import statistics
import subprocess
import sys
import time
from typing import List, Sequence, Tuple


@dataclasses.dataclass
class TimedRun:
    mode: str
    returncodes: List[int]
    seconds: List[float]


def run_mode_once(binary: str, mode: str, fixture: pathlib.Path) -> Tuple[int, float]:
    fixture_path = str(pathlib.Path(fixture).resolve())
    if mode == "evaluator":
        cmd = [binary, fixture_path]
    elif mode == "vm":
        cmd = [binary, "-vm", fixture_path]
    else:
        raise ValueError(f"unsupported mode: {mode}")
    t0 = time.perf_counter()
    completed = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        check=False,
    )
    elapsed = time.perf_counter() - t0
    return completed.returncode, elapsed


def bench_mode(
    binary: str,
    mode: str,
    fixture: pathlib.Path,
    warmup: int,
    iterations: int,
) -> TimedRun:
    for _ in range(max(0, warmup)):
        run_mode_once(binary, mode, fixture)

    codes: List[int] = []
    times_s: List[float] = []
    for _ in range(max(1, iterations)):
        code, elapsed = run_mode_once(binary, mode, fixture)
        codes.append(code)
        times_s.append(elapsed)
    return TimedRun(mode=mode, returncodes=codes, seconds=times_s)


def collect_fixtures(fixtures_dir: pathlib.Path) -> List[pathlib.Path]:
    return sorted(fixtures_dir.rglob("*.ad"))


def fmt_ms(seconds: float) -> str:
    return f"{seconds * 1000.0:.3f} ms"


def summarize_seconds(samples: Sequence[float]) -> Tuple[float, float, float]:
    """Return (median, min, max) in seconds."""
    if not samples:
        return (0.0, 0.0, 0.0)
    return (
        float(statistics.median(samples)),
        float(min(samples)),
        float(max(samples)),
    )


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Benchmark evaluator vs VM execution time for .ad fixtures."
    )
    parser.add_argument(
        "--binary",
        default="./main",
        help="path to interpreter binary (default: ./main)",
    )
    parser.add_argument(
        "--fixtures-dir",
        default="tests/performance/fixtures",
        help="fixtures directory (default: tests/performance/fixtures)",
    )
    parser.add_argument(
        "--warmup",
        type=int,
        default=1,
        help="discarded runs per mode before timing (default: 1)",
    )
    parser.add_argument(
        "--iterations",
        type=int,
        default=10,
        help="timed runs per mode per fixture (default: 10)",
    )
    args = parser.parse_args()

    binary = pathlib.Path(args.binary).expanduser()
    try:
        binary_exe = binary.resolve(strict=True)
    except (OSError, RuntimeError):
        print(f"[error] binary not found or not resolvable: {binary}")
        return 2
    if not binary_exe.is_file() or not os.access(binary_exe, os.X_OK):
        print(f"[error] binary is not an executable file: {binary_exe}")
        return 2

    fixtures_dir = pathlib.Path(args.fixtures_dir)
    if not fixtures_dir.exists():
        print(f"[error] fixtures dir not found: {fixtures_dir}")
        return 2

    fixtures = collect_fixtures(fixtures_dir)
    if not fixtures:
        print(f"[error] no fixtures found in: {fixtures_dir}")
        return 2

    print(
        f"[info] performance: {len(fixtures)} fixture(s), "
        f"warmup={args.warmup}, iterations={args.iterations}"
    )
    print(f"[info] binary: {binary_exe}")

    any_failure = False
    for fixture in fixtures:
        rel = fixture.as_posix()
        print(f"\n[fixture] {rel}")

        ev = bench_mode(str(binary_exe), "evaluator", fixture, args.warmup, args.iterations)
        vm = bench_mode(str(binary_exe), "vm", fixture, args.warmup, args.iterations)

        ev_ok = all(c == 0 for c in ev.returncodes)
        vm_ok = all(c == 0 for c in vm.returncodes)
        if not ev_ok:
            any_failure = True
            print(f"  [error] evaluator: non-zero return code(s): {ev.returncodes}")
        if not vm_ok:
            any_failure = True
            print(f"  [error] vm: non-zero return code(s): {vm.returncodes}")

        ev_med, ev_lo, ev_hi = summarize_seconds(ev.seconds)
        vm_med, vm_lo, vm_hi = summarize_seconds(vm.seconds)

        print(f"  evaluator  median {fmt_ms(ev_med)}  (min {fmt_ms(ev_lo)} .. max {fmt_ms(ev_hi)})")
        print(f"  vm         median {fmt_ms(vm_med)}  (min {fmt_ms(vm_lo)} .. max {fmt_ms(vm_hi)})")

        if ev_med > 0 and vm_med > 0:
            ratio_ev_per_vm = ev_med / vm_med
            if ratio_ev_per_vm >= 1.0:
                print(f"  ratio      evaluator/vm = {ratio_ev_per_vm:.3f}x (higher means VM faster)")
            else:
                print(f"  ratio      vm/evaluator = {1.0 / ratio_ev_per_vm:.3f}x (higher means evaluator faster)")

    if any_failure:
        print("\n[summary] completed with errors (see above)")
        return 1
    print("\n[summary] all timed runs exited with code 0")
    return 0


if __name__ == "__main__":
    sys.exit(main())
