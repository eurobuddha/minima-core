#!/usr/bin/env python3
"""Inventory every tracked Java file. A destination path is not a completed port."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import sys

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / "upstream-study"
PIN = "5564b9d78978efcaf23679190d05253961fc6e69"
REUSE = Path("/Users/eurobuddha/Projects/minima/tools/minima-core-cpp")


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def git(*args):
    return subprocess.check_output(["git", "-C", str(JAVA), *args], text=True).strip()


def inventory():
    if git("rev-parse", "HEAD") != PIN:
        raise ValueError("Upstream checkout changed; review and explicitly update the source pin")
    if git("status", "--porcelain", "--untracked-files=no"):
        raise ValueError("Upstream tracked files are modified")
    paths = sorted(git("ls-files", "*.java").splitlines())
    candidates = {}
    for path in sorted((REUSE / "src").rglob("*")):
        if path.suffix in (".cpp", ".hpp") and "vendor" not in path.parts:
            candidates.setdefault(path.stem, []).append(str(path.relative_to(REUSE)))
    aliases = {"Minima": "main", "TxPoWChecker": "TxPoWValidator", "MMR": "MMRSet",
               "Crypto": "Hash", "ScriptTokenizer": "Tokenizer", "StatementParser": "Parser",
               "GenesisCoin": "Genesis", "GenesisMMR": "Genesis", "GenesisTxPoW": "Genesis"}
    entries = []
    for rel in paths:
        source = JAVA / rel
        target = Path(rel).with_suffix(".cpp")
        existing = ROOT / target
        stem = source.stem
        entries.append({
            "java": rel, "java_sha256": sha(source),
            "cpp": target.as_posix(), "cpp_exists": existing.is_file(),
            "status": "implemented_partial" if existing.is_file() else "unported",
            "parity": "not_verified",
            "reuse_candidates": candidates.get(stem, candidates.get(aliases.get(stem), [])),
            "candidate_evidence": "name_or_explicit_role_match_only; requires_source_review",
            "lines": len(source.read_text().splitlines()),
        })
    assert len({row["cpp"] for row in entries}) == len(entries), "Non-unique C++ destinations"
    return {"upstream": "https://github.com/spartacusrex-minima/minima-core", "commit": PIN,
            "reuse_root": str(REUSE), "files": entries}


def markdown(data):
    rows = data["files"]
    present = sum(row["cpp_exists"] for row in rows)
    lines = ["# Java to C++ file map", "", f"Pinned upstream: `{PIN}`.", "",
             f"{len(rows)} Java files → {len(rows)} unique planned C++ paths. "
             f"{present} C++ files currently exist. Full behavioral parity is not verified.", "",
             "An unported row is a destination assignment, not a generated or compiled implementation.", "",
             "Candidate matches are search leads; they do not certify compatibility or reuse.", "",
             "| Java file | C++ destination | Status | Existing C++ candidates |",
             "|---|---|---|---|"]
    for row in rows:
        candidates = ", ".join(f"`{p}`" for p in row["reuse_candidates"]) or "—"
        lines.append(f"| `{row['java']}` | `{row['cpp']}` | {row['status']} | {candidates} |")
    return "\n".join(lines) + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--check", action="store_true")
    parser.add_argument("--require-complete", action="store_true")
    args = parser.parse_args()
    data = inventory()
    outputs = {ROOT / "source-map.json": json.dumps(data, indent=2) + "\n",
               ROOT / "SOURCE_MAP.md": markdown(data)}
    if args.check or args.require_complete:
        for path, expected in outputs.items():
            if not path.exists() or path.read_text() != expected:
                raise ValueError(f"Stale map: {path}; regenerate after reviewing changes")
    else:
        for path, content in outputs.items():
            path.write_text(content)
    rows = data["files"]
    present = sum(row["cpp_exists"] for row in rows)
    print(f"Mapped {len(rows)} Java files; {present} counterparts exist; {len(rows)-present} unported.")
    if args.require_complete:
        incomplete = [row for row in rows if not row["cpp_exists"] or row["parity"] != "verified"]
        if incomplete:
            print(f"INCOMPLETE: {len(incomplete)} files lack verified parity.", file=sys.stderr)
            return 1
    return 0


if __name__ == "__main__":
    try:
        sys.exit(main())
    except (ValueError, subprocess.CalledProcessError) as error:
        print(error, file=sys.stderr)
        sys.exit(1)
