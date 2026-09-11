#!/usr/bin/env python3
"""Execute the unchanged pinned P2P JUnit tests and their mapped native tests."""
import argparse
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'
TESTS = {'UtilFuncsTest': (2, 2), 'WalkLinksFuncsTest': (3, 3),
         'SwapLinksFunctionsTest': (8, 10)}

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'build')
    build = parser.parse_args().build.resolve()
    oracle = build / 'original-p2p-oracle'
    oracle.mkdir(exist_ok=True)
    package = JAVA / 'test/org/minima/system/network/p2p'
    sources = sorted((JAVA / 'src').rglob('*.java')) + [
        package / (name + '.java') for name in TESTS] + [
        package / 'testingutils/QuickInetLists.java',
        package / 'testingutils/QuickState.java', ROOT / 'support/OriginalP2PTests.java']
    listing = oracle / 'sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    dependencies = f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac', '-cp', dependencies, '-d', str(oracle), '@' + str(listing)], check=True)
    results = []
    for name, (methods, assertions) in TESTS.items():
        expected = dict(passed=True, tests=methods, assertions=assertions, **{'class': name})
        outputs = []
        for command in [['java', '-cp', f'{oracle}:{dependencies}', 'OriginalP2PTests', name],
                        [str(build / name)]]:
            process = subprocess.run(command, capture_output=True, text=True, timeout=30)
            outputs.append(dict(command=command, returncode=process.returncode,
                                stdout=process.stdout, stderr=process.stderr))
        passed = all(p['returncode'] == 0 and not p['stderr'] for p in outputs)
        for output in outputs:
            try:
                passed = (json.loads(output['stdout']) == expected) and passed
            except ValueError:
                passed = False
        results.append(dict(name=name, passed=passed, expected=expected, outputs=outputs))
    report = dict(passed=all(r['passed'] for r in results), original_methods=13,
                  assertions_per_runtime=15, results=results,
                  scope='All active methods and assertions in the three pinned original P2P test classes. '
                        'Java counts assertions from inspected source; native counts executed assertions. '
                        'Random peer ports and secrets remain independently random. '
                        'Passing these source tests is not full production P2P parity.')
    (build / 'original-p2p-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))
    return not report['passed']

if __name__ == '__main__':
    raise SystemExit(main())
