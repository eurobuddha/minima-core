#!/usr/bin/env python3
"""Execute the unchanged pinned base JUnit tests and their mapped native tests."""
import argparse
import json
import re
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'
TESTS = {'MiniNumberTests': (6, 41), 'MiniDataTests': (6, 33)}

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'build')
    build = parser.parse_args().build.resolve()
    oracle = build / 'original-base-oracle'
    oracle.mkdir(exist_ok=True)
    package = JAVA / 'test/org/minima/objects/base'
    sources = sorted((JAVA / 'src').rglob('*.java')) + [
        package / (name + '.java') for name in TESTS] + [ROOT / 'support/OriginalBaseTests.java']
    listing = oracle / 'sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    dependencies = f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac', '-cp', dependencies, '-d', str(oracle), '@' + str(listing)], check=True)
    results = []
    for name, (methods, assertions) in TESTS.items():
        expected = dict(passed=True, tests=methods, assertions=assertions, **{'class': name})
        outputs = []
        for command in [['java', '-cp', f'{oracle}:{dependencies}', 'OriginalBaseTests', name],
                        [str(build / name)]]:
            process = subprocess.run(command, capture_output=True, text=True, timeout=30)
            outputs.append(dict(command=command, returncode=process.returncode,
                                stdout=process.stdout, stderr=process.stderr))
        passed = all(p['returncode'] == 0 and not p['stderr'] for p in outputs)
        observations=[]; source_outputs=[]
        for output in outputs:
            try:
                lines=output['stdout'].splitlines()
                if not lines[-1].startswith('ORIGINAL_BASE_RESULT '):raise ValueError('Missing result marker')
                observation=json.loads(lines[-1].removeprefix('ORIGINAL_BASE_RESULT '))
                passed = all(observation.get(k)==v for k,v in expected.items()) and passed
                observations.append(observation)
                # Preserve the direct runner's source order and every printed observation.
                # JSON field order is not significant.
                original=[]; arrays=randoms=0
                for line in lines[:-1]:
                    if line.startswith('i.getData()'):
                        if not re.fullmatch(r'i.getData\(\)0x1388 resolves as \[B@[0-9a-f]+',line):raise ValueError('Invalid array identity observation')
                        line=line.split(' resolves as ')[0]+' resolves as <array identity>';arrays+=1
                    elif line.startswith('i.getRandomData(1)'):
                        if not re.fullmatch(r'i.getRandomData\(1\)0x1388 resolves as 0x[0-9A-F]{2}',line):raise ValueError('Invalid random-byte observation')
                        line=line.split(' resolves as ')[0]+' resolves as <one random byte>';randoms+=1
                    original.append(line)
                if name=='MiniDataTests' and (arrays!=1 or randoms!=1):raise ValueError('Missing nondeterministic observations')
                source_outputs.append(original)
            except (ValueError,IndexError):
                passed=False
        passed=(len(observations)==2 and len(source_outputs)==2 and observations[0]==observations[1] and source_outputs[0]==source_outputs[1]) and passed
        results.append(dict(name=name, passed=passed, expected=expected, outputs=outputs))
    report = dict(passed=all(r['passed'] for r in results), original_methods=12,
                  assertions_per_runtime=74, results=results,
                  scope='All twelve active methods in pinned MiniNumberTests and MiniDataTests, with 41 and 33 executed assertions respectively on the source paths. Exact annotation membership/options and absent lifecycle methods checked before fresh-instance direct invocation using real JUnit assertions. All source observations compared in order; only validated array identity spelling and one-byte random output are normalized. The original ten-million-byte loop is retained. The source TooBig method executes zero assertions on its actual successful path. No full production API/node parity claim.')
    (build / 'original-base-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))
    return not report['passed']

if __name__ == '__main__':
    raise SystemExit(main())
