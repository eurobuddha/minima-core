#!/usr/bin/env python3
"""Run every active pinned statement command JUnit method and its native counterpart."""
import argparse
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'
TESTS = {'BooleanExpressionTests': 116, 'OperatorExpressionTests': 178}

def execute(command, data=None):
    result = subprocess.run(command, input=data, capture_output=True, text=True, timeout=60)
    return dict(command=command, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'build')
    build = parser.parse_args().build.resolve()
    oracle = build / 'original-expressions-oracle'
    oracle.mkdir(exist_ok=True)
    package = JAVA / 'test/org/minima/tests/kissvm/expressions'
    sources = sorted((JAVA / 'src').rglob('*.java')) + [package / (name + '.java') for name in TESTS]
    sources += [ROOT / 'support/OriginalExpressionsTests.java', ROOT / 'support/KissVMOracle.java']
    listing = oracle / 'sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    dependencies = f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac', '-cp', dependencies, '-d', str(oracle), '@' + str(listing)], check=True)
    java = ['java', '-cp', f'{oracle}:{dependencies}']
    results = []
    for name, assertions in TESTS.items():
        expected = dict(passed=True, tests=2, assertions=assertions, **{'class': name})
        outputs = [execute(java + ['OriginalExpressionsTests', name]), execute([str(build / name)])]
        passed = all(p['returncode'] == 0 and not p['stderr'] for p in outputs)
        for output in outputs:
            try:
                lines = output['stdout'].splitlines()
                if len(lines) != 1 or not lines[0].startswith('ORIGINAL_EXPRESSIONS_RESULT '):
                    raise ValueError('Unexpected test output')
                passed = (json.loads(lines[0].removeprefix('ORIGINAL_EXPRESSIONS_RESULT ')) == expected) and passed
            except ValueError:
                passed = False
        results.append(dict(name=name, passed=passed, outputs=outputs))

    # Known boundary/decimal values supplement source assertions using full probes.
    cases=[('9223372036854775807+1',[2,'9223372036854775808']),
           ('-9223372036854775808-1',[2,'-9223372036854775809']),
           ('-3.14 % 0.5',[2,'-0.14']),('0x01 << -3',[1,'0x00']),
           ('0x01 >> -3',[1,'0x08']),('~0x00FFFF',[1,'0xFF0000']),
           ('NOT TRUE',[8,'FALSE']),('TRUE NXOR FALSE',[8,'FALSE'])]
    data=''.join(f'expr 0x{script.encode().hex()} 1024 1\n' for script,_ in cases)
    observations=[];failures=[];checks=0
    for runtime,command in [('java',java+['KissVMOracle']),('cpp',[str(build/'minima_kissvm')])]:
        output=execute(command,data)
        if output['returncode'] or output['stderr']:raise RuntimeError(output)
        rows=[json.loads(bytes.fromhex(line.removeprefix('0x')).decode()) for line in output['stdout'].splitlines()]
        if len(rows)!=len(cases):raise RuntimeError('Probe count mismatch')
        observations.append(rows)
        for i,(row,(_,expected)) in enumerate(zip(rows,cases)):
            checks+=1
            if len(row)!=13 or row[1][:2]!=expected or row[6] or row[7]:
                failures.append(dict(runtime=runtime,case=i,expected=expected,actual=row))
    for i,(j,n) in enumerate(zip(*observations)):
        if j!=n:failures.append(dict(case=i,java=j,cpp=n))
    supplemental=dict(passed=not failures,comparisons=len(cases),independent_checks=checks,
                      cases=cases,observations=observations,failures=failures)
    report = dict(passed=all(r['passed'] for r in results) and supplemental['passed'], original_methods=4,
                  original_assertions_per_runtime=294, results=results,supplemental=supplemental,
                  scope='Both complete active BooleanExpressionTests and OperatorExpressionTests, four annotated methods and exact source assertions, fixture sizes, shared method contracts, numeric extrema, typed casts, exception categories and commented-case exclusions are retained. Supplemental fixed expressions require known values and full 13-field Java/native observations. MiniNumber limit constants and decimal constructor reuse the native decimal carrier and existing rounding/range validation. Full expression and node parity remain unverified.')
    (build / 'original-expressions-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(dict(passed=report['passed'],original_methods=4,original_assertions_per_runtime=294,
                         classes=[dict(name=r['name'],passed=r['passed']) for r in results],
                         supplemental_comparisons=len(cases),independent_checks=checks,failures=failures),indent=2))
    return not report['passed']

if __name__ == '__main__':
    raise SystemExit(main())
