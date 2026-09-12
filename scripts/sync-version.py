#!/usr/bin/env python3
"""Synchronize Valve's release version mirrors from the VERSION file."""

from __future__ import annotations

import argparse
import re
from pathlib import Path


ROOT = Path(__file__).resolve().parent.parent
VERSION_RE = re.compile(r"^(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)\.(0|[1-9][0-9]*)-([0-9]+)$")
MIRRORS = (
    (Path("meson.build"), re.compile(r"(?<=version: ')[0-9]+\.[0-9]+\.[0-9]+-[0-9]+(?=',)"), 1),
    (
        Path("src/valve/private.h"),
        re.compile(r'(?<=#define VALVE_VERSION ")[0-9]+\.[0-9]+\.[0-9]+-[0-9]+(?=")|(?<=#define PROGRAM_VERSION ")[0-9]+\.[0-9]+\.[0-9]+-[0-9]+(?=")'),
        2,
    ),
    (Path("project.yml"), re.compile(r'(?<=-DVALVE_VERSION=\\")[0-9]+\.[0-9]+\.[0-9]+-[0-9]+(?=\\")'), 1),
    (Path("test/ceedling-gcov.yml"), re.compile(r'(?<=-DVALVE_VERSION=\\")[0-9]+\.[0-9]+\.[0-9]+-[0-9]+(?=\\")'), 1),
)


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true", help="report drift without writing")
    args = parser.parse_args()

    version = (ROOT / "VERSION").read_text(encoding="utf-8").strip()
    if VERSION_RE.fullmatch(version) is None:
        raise SystemExit(f"VERSION is malformed: {version!r}")

    drift = False
    for relative, pattern, expected_count in MIRRORS:
        path = ROOT / relative
        content = path.read_text(encoding="utf-8")
        matches = pattern.findall(content)
        if len(matches) != expected_count:
            raise SystemExit(
                f"{relative}: expected {expected_count} version mirrors, found {len(matches)}"
            )
        current = {match if isinstance(match, str) else "".join(match) for match in matches}
        if current == {version}:
            continue
        drift = True
        if args.check:
            print(f"{relative}: {', '.join(sorted(current))} != {version}")
            continue
        updated, count = pattern.subn(version, content)
        if count != expected_count:
            raise SystemExit(f"{relative}: replacement count changed unexpectedly")
        path.write_text(updated, encoding="utf-8")
        print(f"{relative}: synchronized {count} mirror(s) to {version}")

    if args.check and drift:
        raise SystemExit(1)


if __name__ == "__main__":
    main()
