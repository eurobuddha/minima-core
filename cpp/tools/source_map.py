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


def inventory(provenance=None):
    if (JAVA / ".git").exists():
        if git("rev-parse", "HEAD") != PIN:
            raise ValueError("Upstream checkout changed; review and explicitly update the source pin")
        if git("status", "--porcelain", "--untracked-files=no"):
            raise ValueError("Upstream tracked files are modified")
        paths = sorted(git("ls-files", "*.java").splitlines())
    else:
        # A delivery snapshot has no Git metadata. Its package manifest is the
        # explicit provenance boundary, never an ambient parent Git checkout.
        manifest_path = ROOT.parent.parent / "delivery-manifest.json"
        if provenance is None or not manifest_path.is_file():
            raise ValueError("Source refresh requires the pinned Git checkout; snapshot checks require the delivery manifest")
        manifest = json.loads(manifest_path.read_text())
        if manifest.get("upstream_pin") != PIN:
            raise ValueError("Delivery source pin differs")
        hashes = manifest.get("file_sha256", {})
        for name in ["source-map.json", "SOURCE_MAP.md"]:
            if hashes.get("project/cpp/" + name) != sha(ROOT / name):
                raise ValueError("Delivery mapping hash differs: " + name)
        paths = sorted(str(path.relative_to(JAVA)) for path in JAVA.rglob("*.java"))
        expected = sorted(path.removeprefix("project/upstream-study/") for path in hashes
                          if path.startswith("project/upstream-study/") and path.endswith(".java"))
        if paths != expected:
            raise ValueError("Delivery Java file inventory differs")
        for path in paths:
            if sha(JAVA / path) != hashes["project/upstream-study/" + path]:
                raise ValueError("Delivery Java source hash differs: " + path)
    if provenance is not None and provenance.get("commit") != PIN:
        raise ValueError("Recorded source map pin differs")
    recorded = {row["java"]: row for row in provenance["files"]} if provenance else {}

    candidates = {}
    for path in ([] if provenance is not None else sorted((REUSE / "src").rglob("*"))):
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
        status = "implemented_partial" if existing.is_file() else "unported"
        if rel == "src/org/minima/system/network/p2p/messages/P2PSwapLink.java" and existing.is_file():
            for checked in (source, existing):
                if any(line.strip() and not line.lstrip().startswith("//") for line in checked.read_text().splitlines()):
                    raise ValueError(f"Inactive source unexpectedly contains active code: {checked}")
            status = "inactive_source_preserved"
        entries.append({
            "java": rel, "java_sha256": sha(source),
            "cpp": target.as_posix(), "cpp_exists": existing.is_file(),
            "status": status,
            "parity": "not_verified",
            "reuse_candidates": (recorded.get(rel, {}).get("reuse_candidates", []) if provenance is not None
                                 else candidates.get(stem, candidates.get(aliases.get(stem), []))),
            "candidate_evidence": "name_or_explicit_role_match_only; requires_source_review",
            "lines": len(source.read_text().splitlines()),
        })
    assert len({row["cpp"] for row in entries}) == len(entries), "Non-unique C++ destinations"
    return {"upstream": "https://github.com/spartacusrex-minima/minima-core", "commit": PIN,
            "reuse_root": provenance["reuse_root"] if provenance is not None else str(REUSE), "files": entries}


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
    # Search leads are historical provenance, not machine-local parity evidence.
    # Refresh mode still searches the catalogue; check mode preserves recorded leads.
    checking = args.check or args.require_complete
    provenance = json.loads((ROOT / "source-map.json").read_text()) if checking else None
    data = inventory(provenance)
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
