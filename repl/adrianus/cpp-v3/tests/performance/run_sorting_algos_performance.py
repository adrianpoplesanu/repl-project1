#!/usr/bin/env python3
"""
Performance harness: wall-clock time for bootstrap sorting methods.

Same binary / subprocess contract as tests/performance/run_performance.py.
Each fixture in tests/performance/algos sorts the same 30-element array with
one _AdAlgos_* class from bootstrap/algos.ad. After per-fixture evaluator-vs-VM
timings, prints a ranking of sorting methods by median time.
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
from typing import Dict, List, Sequence, Tuple


@dataclasses.dataclass
class TimedRun:
    mode: str
    returncodes: List[int]
    seconds: List[float]


def run_mode_once(
    binary: str, mode: str, fixture: pathlib.Path, no_mid_gc: bool = False
) -> Tuple[int, float]:
    fixture_path = str(pathlib.Path(fixture).resolve())
    if mode == "evaluator":
        cmd = [binary, fixture_path]
    elif mode == "vm":
        cmd = [binary, "-vm"]
        if no_mid_gc:
            cmd.append("-no-mid-gc")
        cmd.append(fixture_path)
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
    no_mid_gc: bool = False,
) -> TimedRun:
    for _ in range(max(0, warmup)):
        run_mode_once(binary, mode, fixture, no_mid_gc)

    codes: List[int] = []
    times_s: List[float] = []
    for _ in range(max(1, iterations)):
        code, elapsed = run_mode_once(binary, mode, fixture, no_mid_gc)
        codes.append(code)
        times_s.append(elapsed)
    return TimedRun(mode=mode, returncodes=codes, seconds=times_s)


def collect_fixtures(fixtures_dir: pathlib.Path) -> List[pathlib.Path]:
    return sorted(
        p for p in fixtures_dir.rglob("*.ad") if not p.name.startswith("_")
    )


def bootstrap_baseline_path(fixtures_dir: pathlib.Path) -> pathlib.Path | None:
    candidate = fixtures_dir / "_bootstrap_only.ad"
    return candidate if candidate.is_file() else None


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


def algo_name(fixture: pathlib.Path) -> str:
    return fixture.stem


def print_algo_ranking(title: str, medians: Dict[str, float]) -> None:
    ranked = sorted(medians.items(), key=lambda item: item[1])
    if not ranked:
        return
    print(f"\n[ranking] {title}")
    fastest = ranked[0][1]
    for i, (name, med) in enumerate(ranked, start=1):
        vs_fastest = ""
        if fastest > 0 and med > 0:
            vs_fastest = f"  ({med / fastest:.3f}x vs fastest)"
        print(f"  {i}. {name:16} median {fmt_ms(med)}{vs_fastest}")


def main() -> int:
    parser = argparse.ArgumentParser(
        description=(
            "Benchmark bootstrap sorting methods (evaluator vs VM) on a 30-element array."
        )
    )
    parser.add_argument(
        "--binary",
        default="./main",
        help="path to interpreter binary (default: ./main)",
    )
    parser.add_argument(
        "--fixtures-dir",
        default="tests/performance/algos",
        help="fixtures directory (default: tests/performance/algos)",
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
    parser.add_argument(
        "--no-mid-gc",
        action="store_true",
        help="disable VM mid-run mark/sweep for A/B comparison (end-of-run free still runs)",
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
        f"[info] sorting algos: {len(fixtures)} fixture(s), "
        f"warmup={args.warmup}, iterations={args.iterations}, "
        f"vm_mid_run_gc={not args.no_mid_gc}"
    )
    print(f"[info] binary: {binary_exe}")
    print("[info] input: 30-element reverse-sorted array [30..1], 400 repeats per run")

    any_failure = False
    ev_medians: Dict[str, float] = {}
    vm_medians: Dict[str, float] = {}
    ev_baseline = 0.0
    vm_baseline = 0.0

    baseline = bootstrap_baseline_path(fixtures_dir)
    if baseline is not None:
        print(f"\n[baseline] {baseline.as_posix()} (process + bootstrap, no sort)")
        ev_b = bench_mode(
            str(binary_exe), "evaluator", baseline, args.warmup, args.iterations, args.no_mid_gc
        )
        vm_b = bench_mode(
            str(binary_exe), "vm", baseline, args.warmup, args.iterations, args.no_mid_gc
        )
        ev_baseline, ev_blo, ev_bhi = summarize_seconds(ev_b.seconds)
        vm_baseline, vm_blo, vm_bhi = summarize_seconds(vm_b.seconds)
        print(
            f"  evaluator  median {fmt_ms(ev_baseline)}  "
            f"(min {fmt_ms(ev_blo)} .. max {fmt_ms(ev_bhi)})"
        )
        print(
            f"  vm         median {fmt_ms(vm_baseline)}  "
            f"(min {fmt_ms(vm_blo)} .. max {fmt_ms(vm_bhi)})"
        )
        if ev_baseline > 0 and vm_baseline > 0:
            print(
                f"  note       VM bootstrap/compile overhead vs evaluator: "
                f"{vm_baseline / ev_baseline:.3f}x"
            )

    for fixture in fixtures:
        name = algo_name(fixture)
        rel = fixture.as_posix()
        print(f"\n[fixture] {rel}")

        ev = bench_mode(
            str(binary_exe), "evaluator", fixture, args.warmup, args.iterations, args.no_mid_gc
        )
        vm = bench_mode(
            str(binary_exe), "vm", fixture, args.warmup, args.iterations, args.no_mid_gc
        )

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
        ev_medians[name] = ev_med
        vm_medians[name] = vm_med

        print(f"  evaluator  median {fmt_ms(ev_med)}  (min {fmt_ms(ev_lo)} .. max {fmt_ms(ev_hi)})")
        print(f"  vm         median {fmt_ms(vm_med)}  (min {fmt_ms(vm_lo)} .. max {fmt_ms(vm_hi)})")
        if ev_baseline > 0 or vm_baseline > 0:
            ev_net = max(0.0, ev_med - ev_baseline)
            vm_net = max(0.0, vm_med - vm_baseline)
            print(f"  net        evaluator {fmt_ms(ev_net)}  vm {fmt_ms(vm_net)}  (minus bootstrap baseline)")

        if ev_med > 0 and vm_med > 0:
            ratio_ev_per_vm = ev_med / vm_med
            if ratio_ev_per_vm >= 1.0:
                print(f"  ratio      evaluator/vm = {ratio_ev_per_vm:.3f}x (higher means VM faster)")
            else:
                print(f"  ratio      vm/evaluator = {1.0 / ratio_ev_per_vm:.3f}x (higher means evaluator faster)")

    print_algo_ranking("evaluator (faster first)", ev_medians)
    print_algo_ranking("vm (faster first)", vm_medians)

    if any_failure:
        print("\n[summary] completed with errors (see above)")
        return 1
    print("\n[summary] all timed runs exited with code 0")
    return 0


if __name__ == "__main__":
    sys.exit(main())
