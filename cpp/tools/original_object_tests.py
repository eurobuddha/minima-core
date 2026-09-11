#!/usr/bin/env python3
"""Execute the unchanged pinned object JUnit tests and their mapped native tests."""
import argparse
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'
TESTS = {'AddressTests': (3, 10), 'CoinTests': (3, 5), 'TokenTests': (2, 5)}

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'build')
    build = parser.parse_args().build.resolve()
    oracle = build / 'original-object-oracle'
    oracle.mkdir(exist_ok=True)
    package = JAVA / 'test/org/minima/objects'
    sources = sorted((JAVA / 'src').rglob('*.java')) + [
        package / (name + '.java') for name in TESTS] + [ROOT / 'support/OriginalObjectTests.java']
    listing = oracle / 'sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    dependencies = f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac', '-cp', dependencies, '-d', str(oracle), '@' + str(listing)], check=True)
    results = []
    for name, (methods, assertions) in TESTS.items():
        expected = dict(passed=True, tests=methods, assertions=assertions, **{'class': name})
        outputs = []
        for command in [['java', '-cp', f'{oracle}:{dependencies}', 'OriginalObjectTests', name],
                        [str(build / name)]]:
            process = subprocess.run(command, capture_output=True, text=True, timeout=30)
            outputs.append(dict(command=command, returncode=process.returncode,
                                stdout=process.stdout, stderr=process.stderr))
        passed = all(p['returncode'] == 0 and not p['stderr'] for p in outputs)
        observations=[]; source_outputs=[]
        for output in outputs:
            try:
                lines=output['stdout'].splitlines()
                if not lines[-1].startswith('ORIGINAL_OBJECT_RESULT '):raise ValueError('Missing result marker')
                observation=json.loads(lines[-1].removeprefix('ORIGINAL_OBJECT_RESULT '))
                passed = all(observation.get(k)==v for k,v in expected.items()) and passed
                if not observation.get('wire') or not isinstance(observation.get('decoded'),dict):passed=False
                observations.append(observation)
                # Preserve the direct runner's source order and every printed observation.
                # JSON field order is not significant.
                original=[]
                for line in lines[:-1]:
                    prefix,separator,payload=line.partition('{')
                    original.append(prefix+json.dumps(json.loads(separator+payload),sort_keys=True) if separator else line)
                source_outputs.append(original)
            except (ValueError,IndexError):
                passed=False
        passed=(len(observations)==2 and len(source_outputs)==2 and observations[0]==observations[1] and source_outputs[0]==source_outputs[1]) and passed
        results.append(dict(name=name, passed=passed, expected=expected, outputs=outputs))
    report = dict(passed=all(r['passed'] for r in results), original_methods=8,
                  assertions_per_runtime=20, supplemental_wire_and_decoded_comparisons=6, results=results,
                  scope='All eight active methods and twenty successful-path assertions in pinned AddressTests, CoinTests and TokenTests. Java counts source assertions; native counts executed assertions. The exact annotated method set and default test options are checked before direct invocation on fresh instances using real JUnit assertions. All original printed observations are compared in execution order; JSON key order is normalized. Supplemental exact wire and decoded JSON comparisons for one original sample per class are separate from source assertions. Full production API and node parity remain unverified.')
    (build / 'original-object-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))
    return not report['passed']

if __name__ == '__main__':
    raise SystemExit(main())
