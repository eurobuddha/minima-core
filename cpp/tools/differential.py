#!/usr/bin/env python3
"""Compare primitive results and wire bytes with freshly compiled upstream Java."""
import argparse
import json
from pathlib import Path
import random
import subprocess

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / "upstream-study"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--build", type=Path, default=ROOT / "build")
    args = parser.parse_args()
    build = args.build.resolve()
    subprocess.run(["python3", str(ROOT / "tools/source_map.py"), "--check"], check=True)
    oracle = build / "java-oracle"
    oracle.mkdir(parents=True, exist_ok=True)
    jar = JAVA / "jar/minima.jar"
    sources = [JAVA / "src/org/minima/utils/Streamable.java"]
    sources += [JAVA / f"src/org/minima/objects/base/{name}.java"
                for name in ("MiniByte", "MiniData", "MiniNumber", "MiniString")]
    subprocess.run(["javac", "-cp", str(jar), "-d", str(oracle),
                    *map(str, sources), str(ROOT / "support/PrimitiveOracle.java")], check=True)
    cases = [f"byte {value}" for value in range(-512, 513)]
    numbers = ["0", "0.00", "1", "-1", "127", "128", "255", "256", "-128", "-129",
               "0.1", "1.00", "1000", "1E3", "1E-44", "1E-45", "-1E-45",
               "18446744073709551615", "18446744073709551616", "-18446744073709551616"]
    cases += [f"number {n}" for n in numbers]
    cases += ["sigdig 12.34 0", "sigdig 12.34 3", "floor 255.9", "ceil 255.1",
              "floor -255.1", "ceil -255.9", "div 100 4", "div 1 3", "div -1 3", "div 1 0"]
    cases += [f"data {s}" for s in ["0x", "0x00", "0x0001", "0xFF", "0xABC", "0xINVALID"]]
    cases += ["less 0x0001 0x02", "less 0x01 0x0001", "less 0x 0x00"]
    cases += ["string 0x", "string 0x68656C6C6F", "string 0xE282AC", "string 0xF09F90BC"]
    rng = random.Random(5564)
    for _ in range(100):
        a, b = rng.randint(-1000000, 1000000), rng.randint(-1000000, 1000000)
        cases += [f"add {a}.125 {b}.5", f"sub {a}.25 {b}.125", f"mul {a} {b}"]
    for byte in range(256): cases.append(f"string 0x{byte:02X}")
    for _ in range(2000): cases.append("string 0x" + rng.randbytes(rng.randrange(1,10)).hex())
    for sequence in ["EDA080","EDA0","EDBFBF","F4908080","E08080","F0808080","F09F90","C080"]: cases.append("string 0x"+sequence)
    for n in numbers + ["9223372036854775808","4294967296","-4294967297",".0","-0E2","+1",".","1e","1e+","--1"]:
        cases += [f"long {n}", f"int {n}", f"sqrt {n}"]
    for n in ["0","0.00","1","1.00","2","-2","0.1","1.23456789","1E10","1E-10"]:
        for power in [-10,-3,-1,0,1,2,3,10]:cases.append(f"pow {n} {power}")
    stdin = "\n".join(cases) + "\n"
    def run(command):
        result = subprocess.run(command, input=stdin, text=True, capture_output=True, check=True, timeout=60)
        return result.stdout.splitlines()
    expected = run(["java", "-cp", f"{oracle}:{jar}", "PrimitiveOracle"])
    actual = run([str(build / "minima_primitives")])
    if len(expected) != len(cases) or len(actual) != len(cases):
        raise RuntimeError("Oracle/probe returned an unexpected number of results")
    failures = [{"case": case, "java": j, "cpp": c}
                for case, j, c in zip(cases, expected, actual) if j != c]
    report = {"cases": len(cases), "passed": len(cases)-len(failures), "failures": failures,
              "scope": "Selected primitive operations and wire bytes, not full node parity"}
    (build / "differential-results.json").write_text(json.dumps(report, indent=2) + "\n")
    print(json.dumps({**report, "failures": failures[:15]}, indent=2))
    return 1 if failures else 0


if __name__ == "__main__":
    raise SystemExit(main())
