#!/usr/bin/env python3
"""
Differential parity runner scaffold for Evaluator vs VM.

Current behavior:
- Runs each fixture with evaluator mode and VM mode.
- Compares status and normalized output snapshots.
- Produces a concise pass/fail report.

Important:
- Evaluator/VM currently expose different default output behavior for file execution.
- This runner includes normalization hooks and an output adapter contract
  (`PARITY_RESULT:` lines) to support stricter value parity once wired.
"""

from __future__ import annotations

import argparse
import dataclasses
import pathlib
import re
import subprocess
import sys
from typing import List, Tuple


# Keep this intentionally small now that most debug prints are disabled.
# Add patterns only for stable, non-semantic banner lines.
IGNORE_LINE_PATTERNS = [
    r"^Ad interpreter .*",
]

PARITY_RESULT_PREFIX = "PARITY_RESULT:"


@dataclasses.dataclass
class RunResult:
    mode: str
    returncode: int
    stdout: str
    stderr: str


def normalize_lines(raw: str, ignore_patterns: List[str]) -> List[str]:
    lines = [line.rstrip() for line in raw.splitlines()]
    normalized: List[str] = []
    for line in lines:
        if not line:
            continue
        ignored = False
        for pattern in ignore_patterns:
            if re.match(pattern, line):
                ignored = True
                break
        if not ignored:
            normalized.append(line)
    return normalized


def extract_semantic_payload(lines: List[str]) -> str:
    for line in reversed(lines):
        if line.startswith(PARITY_RESULT_PREFIX):
            return line[len(PARITY_RESULT_PREFIX) :].strip()
    if lines:
        return lines[-1]
    return ""


def run_mode(binary: str, mode: str, fixture: pathlib.Path) -> RunResult:
    if mode == "evaluator":
        cmd = [binary, str(fixture)]
    elif mode == "vm":
        cmd = [binary, "-vm", str(fixture)]
    else:
        raise ValueError(f"unsupported mode: {mode}")
    completed = subprocess.run(
        cmd,
        capture_output=True,
        text=True,
        check=False,
    )
    return RunResult(
        mode=mode,
        returncode=completed.returncode,
        stdout=completed.stdout,
        stderr=completed.stderr,
    )


def compare_pair(
    evaluator: RunResult, vm: RunResult, ignore_patterns: List[str]
) -> Tuple[bool, List[str]]:
    problems: List[str] = []
    if evaluator.returncode != vm.returncode:
        problems.append(
            f"return code mismatch evaluator={evaluator.returncode} vm={vm.returncode}"
        )

    ev_lines = normalize_lines(evaluator.stdout, ignore_patterns)
    vm_lines = normalize_lines(vm.stdout, ignore_patterns)
    ev_payload = extract_semantic_payload(ev_lines)
    vm_payload = extract_semantic_payload(vm_lines)
    if ev_payload != vm_payload:
        problems.append(
            f"payload mismatch evaluator='{ev_payload}' vm='{vm_payload}'"
        )

    if bool(evaluator.stderr.strip()) != bool(vm.stderr.strip()):
        problems.append("stderr presence mismatch")

    return (len(problems) == 0, problems)


def collect_fixtures(fixtures_dir: pathlib.Path) -> List[pathlib.Path]:
    return sorted(fixtures_dir.rglob("*.ad"))


def main() -> int:
    parser = argparse.ArgumentParser(description="Run evaluator-vs-vm parity checks.")
    parser.add_argument(
        "--binary",
        default="./main",
        help="path to interpreter binary (default: ./main)",
    )
    parser.add_argument(
        "--fixtures-dir",
        default="tests/parity/fixtures",
        help="fixtures directory (default: tests/parity/fixtures)",
    )
    parser.add_argument(
        "--ignore-pattern",
        action="append",
        default=[],
        help="extra regex pattern to ignore output lines (repeatable)",
    )
    args = parser.parse_args()

    fixtures_dir = pathlib.Path(args.fixtures_dir)
    if not fixtures_dir.exists():
        print(f"[error] fixtures dir not found: {fixtures_dir}")
        return 2

    fixtures = collect_fixtures(fixtures_dir)
    if not fixtures:
        print(f"[error] no fixtures found in: {fixtures_dir}")
        return 2

    effective_ignore_patterns = IGNORE_LINE_PATTERNS + args.ignore_pattern

    print(f"[info] running parity on {len(fixtures)} fixture(s)")
    failures = 0
    for fixture in fixtures:
        ev = run_mode(args.binary, "evaluator", fixture)
        vm = run_mode(args.binary, "vm", fixture)
        ok, problems = compare_pair(ev, vm, effective_ignore_patterns)
        rel = fixture.as_posix()
        if ok:
            print(f"[PASS] {rel}")
        else:
            failures += 1
            print(f"[FAIL] {rel}")
            for p in problems:
                print(f"  - {p}")

    if failures:
        print(f"[summary] {failures} failure(s)")
        return 1
    print("[summary] all fixtures passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
