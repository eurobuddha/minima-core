#!/usr/bin/env python3
"""Run every active pinned signature JUnit method and its native counterpart."""
import argparse
import json
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'
TESTS = {'SIGNEDBYTests': 13, 'MULTISIGTests': 80, 'CHECKSIGTests': 55}

def execute(command, data=None):
    result = subprocess.run(command, input=data, capture_output=True, text=True, timeout=60)
    return dict(command=command, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'build')
    build = parser.parse_args().build.resolve()
    oracle = build / 'original-signature-oracle'
    oracle.mkdir(exist_ok=True)
    package = JAVA / 'test/org/minima/tests/kissvm/functions/sigs'
    sources = sorted((JAVA / 'src').rglob('*.java')) + [package / (name + '.java') for name in TESTS]
    sources += [ROOT / 'support/OriginalSignatureTests.java', ROOT / 'support/ObjectOracle.java']
    listing = oracle / 'sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    dependencies = f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac', '-cp', dependencies, '-d', str(oracle), '@' + str(listing)], check=True)
    java = ['java', '-cp', f'{oracle}:{dependencies}']
    results = []
    for name, assertions in TESTS.items():
        expected = dict(passed=True, tests=3, assertions=assertions, **{'class': name})
        outputs = [execute(java + ['OriginalSignatureTests', name]), execute([str(build / name)])]
        passed = all(p['returncode'] == 0 and not p['stderr'] for p in outputs)
        for output in outputs:
            try:
                lines = output['stdout'].splitlines()
                if len(lines) != 1 or not lines[0].startswith('ORIGINAL_SIGS_RESULT '):
                    raise ValueError('Unexpected test output')
                passed = (json.loads(lines[0].removeprefix('ORIGINAL_SIGS_RESULT ')) == expected) and passed
            except ValueError:
                passed = False
        results.append(dict(name=name, passed=passed, outputs=outputs))

    # Existing object probes: unchanged 16-key/two-level trees, deterministic
    # seeds/messages, exact public keys and signature bytes, both writer outputs.
    commands = [java + ['ObjectOracle'], [str(build / 'minima_objects')]]
    seeds = [bytes(range(32)), bytes(range(64))]
    messages = ['0x' + bytes(range(32)).hex(), '0x' + bytes(range(64, 128)).hex()]
    cases = [f'treekey 0x{seed.hex()} 16 2 0 1 {message}' for seed, message in zip(seeds, messages)]
    generated = []; failures = []; independent_checks = 0
    def probe(command, cases):
        output = execute(command, '\n'.join(cases) + '\n')
        if output['returncode'] or output['stderr']:raise RuntimeError(output)
        rows = output['stdout'].splitlines()
        if len(rows) != len(cases):raise RuntimeError('Probe output count changed')
        return rows
    for command in commands:
        rows = probe(command, cases);generated.append(rows)
        for row in rows:
            fields = row.split('|');independent_checks += 1
            if (len(fields) != 5 or not re.fullmatch(r'0x[0-9A-F]{64}', fields[0]) or
                fields[1:3] != ['256', '1'] or fields[4] != 'true' or
                not re.fullmatch(r'0x(?:[0-9A-F]{2})+', fields[3])):
                failures.append(dict(stage='generation', row=row))
    if generated[0] != generated[1]:failures.append(dict(stage='wire comparison', java=generated[0], cpp=generated[1]))
    verification_cases = []; expected = []
    if not failures:
        for writer_rows in generated:
            for row, message in zip(writer_rows, messages):
                pubkey, _, _, signature, _ = row.split('|')
                verification_cases += [f'treeverify {pubkey} {message} {signature}',
                                       f'treeverify {pubkey} {message}FF {signature}',
                                       f'treeverify 0x00 {message} {signature}']
                expected += ['true', 'false', 'false']
    verified = []
    for command in commands:
        rows = probe(command, verification_cases) if verification_cases else []
        verified.append(rows)
        for index, (actual, wanted) in enumerate(zip(rows, expected)):
            independent_checks += 1
            if actual != wanted:failures.append(dict(stage='cross verification', case=index, actual=actual, expected=wanted))
    if verified[0] != verified[1]:failures.append(dict(stage='verification comparison', java=verified[0], cpp=verified[1]))
    supplemental = dict(passed=not failures, failures=failures, comparisons=2+len(verification_cases),
                        independent_checks=independent_checks, cases=cases, generated=generated,
                        verification_cases=verification_cases, verified=verified)
    report = dict(passed=all(r['passed'] for r in results) and supplemental['passed'], original_methods=9,
                  original_assertions_per_runtime=148, results=results, supplemental=supplemental,
                  scope='All active original fixtures and assertions, with random values generated independently per runtime. SIGNEDBY membership; MULTISIG retains eleven sizes, all 33 valid cases and the duplicated negative fixture; CHECKSIG retains two real TreeKey(seed,16,2) trees and all eight valid/mismatched combinations, plus 35 rejection assertions. Supplemental fixed 32/64-byte seeds and messages compare public keys/signature bytes and cross-verify both writers with valid/changed data and wrong public key. No generic ERROR replies allowed. No full signature API, malformed-wire, aliasing/concurrency or node parity claim.')
    (build / 'original-signature-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(dict(passed=report['passed'], original_methods=9, original_assertions_per_runtime=148,
                         classes=[dict(name=r['name'], passed=r['passed']) for r in results],
                         supplemental_comparisons=supplemental['comparisons'], independent_checks=independent_checks,
                         failures=failures), indent=2))
    return not report['passed']

if __name__ == '__main__':
    raise SystemExit(main())
