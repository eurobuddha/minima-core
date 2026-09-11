#!/usr/bin/env python3
"""Run every active pinned state JUnit method and its native counterpart."""
import argparse
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'
TESTS = {'PREVSTATETests': 139, 'STATETests': 139, 'SAMESTATETests': 281}

def execute(command, data=None):
    result = subprocess.run(command, input=data, capture_output=True, text=True, timeout=60)
    return dict(command=command, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'build')
    build = parser.parse_args().build.resolve()
    oracle = build / 'original-state-oracle'
    oracle.mkdir(exist_ok=True)
    package = JAVA / 'test/org/minima/tests/kissvm/functions/state'
    sources = sorted((JAVA / 'src').rglob('*.java')) + [package / (name + '.java') for name in TESTS]
    sources += [ROOT / 'support/OriginalStateTests.java']
    listing = oracle / 'sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    dependencies = f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac', '-cp', dependencies, '-d', str(oracle), '@' + str(listing)], check=True)
    java = ['java', '-cp', f'{oracle}:{dependencies}']
    results = []
    for name, assertions in TESTS.items():
        expected = dict(passed=True, tests=3, assertions=assertions, **{'class': name})
        outputs = [execute(java + ['OriginalStateTests', name]), execute([str(build / name)])]
        passed = all(p['returncode'] == 0 and not p['stderr'] for p in outputs)
        for output in outputs:
            try:
                lines = output['stdout'].splitlines()
                if len(lines) != 1 or not lines[0].startswith('ORIGINAL_STATE_RESULT '):
                    raise ValueError('Unexpected test output')
                passed = (json.loads(lines[0].removeprefix('ORIGINAL_STATE_RESULT ')) == expected) and passed
            except ValueError:
                passed = False
        results.append(dict(name=name, passed=passed, outputs=outputs))

    report = dict(passed=all(r['passed'] for r in results), original_methods=9,
                  original_assertions_per_runtime=559, results=results,
                  scope='All nine active pinned methods and fixtures: 64 current and 64 previous ports across four value types; 64 equal and 64 different singleton state comparisons; original arity, type, missing-state and range rejection checks. Native casts and exception class checks retained. Direct Java runner verifies annotation membership/defaults and invokes actual JUnit assertions on fresh instances. No source loops reduced. Original SAMESTATE valid cases are singleton ranges, not multi-port range coverage. No full state API, mutation/aliasing or node parity claim.')
    (build / 'original-state-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))
    return not report['passed']

if __name__ == '__main__':
    raise SystemExit(main())
