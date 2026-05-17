#!/usr/bin/env python3
"""Wall-clock timing: permutations_backtrack_assign.py vs main vs main -vm."""

from __future__ import annotations

import argparse
import os
import pathlib
import statistics
import subprocess
import sys
import time
from typing import List, Sequence, Tuple


def fmt_ms(seconds: float) -> str:
    return f"{seconds * 1000.0:.3f} ms"


def summarize(samples: Sequence[float]) -> Tuple[float, float, float]:
    if not samples:
        return (0.0, 0.0, 0.0)
    return (
        float(statistics.median(samples)),
        float(min(samples)),
        float(max(samples)),
    )


def run_once(cmd: List[str]) -> Tuple[int, float]:
    t0 = time.perf_counter()
    completed = subprocess.run(cmd, capture_output=True, text=True, check=False)
    return completed.returncode, time.perf_counter() - t0


def bench(cmd: List[str], warmup: int, iterations: int) -> Tuple[List[int], List[float]]:
    for _ in range(max(0, warmup)):
        run_once(cmd)
    codes: List[int] = []
    times: List[float] = []
    for _ in range(max(1, iterations)):
        code, elapsed = run_once(cmd)
        codes.append(code)
        times.append(elapsed)
    return codes, times


def main() -> int:
    here = pathlib.Path(__file__).resolve().parent
    fixtures = here / "fixtures"
    parser = argparse.ArgumentParser(
        description="Compare permutations_backtrack_assign: Python vs evaluator vs VM."
    )
    parser.add_argument("--binary", default="./main", help="interpreter binary (default: ./main)")
    parser.add_argument("--warmup", type=int, default=1)
    parser.add_argument("--iterations", type=int, default=5)
    args = parser.parse_args()

    binary = pathlib.Path(args.binary).expanduser()
    try:
        binary_exe = binary.resolve(strict=True)
    except (OSError, RuntimeError):
        print(f"[error] binary not found: {binary}", file=sys.stderr)
        return 2
    if not binary_exe.is_file() or not os.access(binary_exe, os.X_OK):
        print(f"[error] not executable: {binary_exe}", file=sys.stderr)
        return 2

    py_script = fixtures / "permutations_backtrack_assign.py"
    ad_fixture = fixtures / "permutations_backtrack_assign.ad"
    if not py_script.is_file() or not ad_fixture.is_file():
        print("[error] missing permutations_backtrack_assign.py or .ad", file=sys.stderr)
        return 2

    py_cmd = [sys.executable, str(py_script.resolve())]
    ev_cmd = [str(binary_exe), str(ad_fixture.resolve())]
    vm_cmd = [str(binary_exe), "-vm", str(ad_fixture.resolve())]

    print(
        f"[info] warmup={args.warmup}, iterations={args.iterations}, binary={binary_exe}"
    )
    print(f"[info] fixture: {ad_fixture.relative_to(here.parent.parent)}")

    modes = [
        ("python", py_cmd),
        ("evaluator", ev_cmd),
        ("vm", vm_cmd),
    ]

    any_failure = False
    medians: dict[str, float] = {}
    for name, cmd in modes:
        codes, times = bench(cmd, args.warmup, args.iterations)
        ok = all(c == 0 for c in codes)
        if not ok:
            any_failure = True
            print(f"\n[{name}] ERROR return code(s): {codes}")
            continue
        med, lo, hi = summarize(times)
        medians[name] = med
        print(f"\n[{name}]")
        print(f"  median {fmt_ms(med)}  (min {fmt_ms(lo)} .. max {fmt_ms(hi)})")

    if len(medians) == 3:
        py_m, ev_m, vm_m = medians["python"], medians["evaluator"], medians["vm"]
        print("\n[ratios vs python]")
        if py_m > 0:
            print(f"  evaluator/python = {ev_m / py_m:.3f}x")
            print(f"  vm/python        = {vm_m / py_m:.3f}x")
        if ev_m > 0 and vm_m > 0:
            r = ev_m / vm_m
            if r >= 1.0:
                print(f"  evaluator/vm     = {r:.3f}x (VM faster)")
            else:
                print(f"  vm/evaluator     = {1.0 / r:.3f}x (evaluator faster)")

    return 1 if any_failure else 0


if __name__ == "__main__":
    sys.exit(main())
