#!/usr/bin/env python3
"""Run every active pinned transaction verify JUnit method and its native counterpart."""
import argparse
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'
TESTS = {'VERIFYINTests': 28, 'VERIFYOUTTests': 28}

def execute(command, data=None):
    result = subprocess.run(command, input=data, capture_output=True, text=True, timeout=60)
    return dict(command=command, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'build')
    build = parser.parse_args().build.resolve()
    oracle = build / 'original-verify-oracle'
    oracle.mkdir(exist_ok=True)
    package = JAVA / 'test/org/minima/tests/kissvm/functions/txn'
    sources = sorted((JAVA / 'src').rglob('*.java')) + [package / ('input' if name.startswith('VERIFYIN') else 'output') / (name + '.java') for name in TESTS]
    sources += [ROOT / 'support/OriginalVerifyTests.java', ROOT / 'support/KissVMOracle.java']
    listing = oracle / 'sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    dependencies = f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac', '-cp', dependencies, '-d', str(oracle), '@' + str(listing)], check=True)
    java = ['java', '-cp', f'{oracle}:{dependencies}']
    results = []
    for name, assertions in TESTS.items():
        expected = dict(passed=True, tests=3, assertions=assertions, **{'class': name})
        outputs = [execute(java + ['OriginalVerifyTests', name]), execute([str(build / name)])]
        passed = all(p['returncode'] == 0 and not p['stderr'] for p in outputs)
        for output in outputs:
            try:
                lines = output['stdout'].splitlines()
                if len(lines) != 1 or not lines[0].startswith('ORIGINAL_VERIFY_RESULT '):
                    raise ValueError('Unexpected test output')
                passed = (json.loads(lines[0].removeprefix('ORIGINAL_VERIFY_RESULT ')) == expected) and passed
            except ValueError:
                passed = False
        results.append(dict(name=name, passed=passed, outputs=outputs))

    # Known values supplement original fixtures with optional keep-state behavior.
    expressions=[];expected=[]
    for fn,amounts in [('VERIFYIN',['7','25']),('VERIFYOUT',['5','12.5'])]:
        for index,amount in enumerate(amounts):
            token='0x00' if index==0 else 'tok'
            for tail in ([''] if fn=='VERIFYIN' else ['', ' TRUE', ' FALSE']):
                for field in ['match','address','amount','token']:
                    addr='0x99' if field=='address' else f'0x2{index}'
                    amt='99' if field=='amount' else amount
                    tok='0x99' if field=='token' else token
                    expressions.append(f'{fn}({index} {addr} {amt} {tok}{tail})')
                    keep=not tail or (tail==' TRUE')==(index==0)
                    expected.append([8,'TRUE' if field=='match' and keep else 'FALSE'])
    data=''.join(f'expr 0x{expression.encode().hex()} 1024 1\n' for expression in expressions)
    observations=[];failures=[];checks=0
    for runtime,command in [('java',java+['KissVMOracle']),('cpp',[str(build/'minima_kissvm')])]:
        output=execute(command,data)
        if output['returncode'] or output['stderr']:raise RuntimeError(output)
        rows=[json.loads(bytes.fromhex(line.removeprefix('0x')).decode()) for line in output['stdout'].splitlines()]
        if len(rows)!=len(expressions):raise RuntimeError('Probe count mismatch')
        observations.append(rows)
        for i,(row,value) in enumerate(zip(rows,expected)):
            checks+=1
            if len(row)!=13 or row[1][:2]!=value or row[6] or row[7]:failures.append(dict(runtime=runtime,case=i,expected=value,actual=row))
    for i,(j,n) in enumerate(zip(*observations)):
        if j!=n:failures.append(dict(case=i,java=j,cpp=n))
    supplemental=dict(passed=not failures,comparisons=len(expressions),independent_checks=checks,
                      expressions=expressions,observations=observations,failures=failures)
    report = dict(passed=all(r['passed'] for r in results) and supplemental['passed'], original_methods=6,
                  original_assertions_per_runtime=56, results=results,supplemental=supplemental,
                  scope='Both original VERIFY test files, six annotated methods and newSimpleAddress helpers retain all active fixtures and 56 assertions per runtime. Source valid and invalid token assignments are preserved. Supplemental fixed fixtures exercise field mismatches, scaled token amounts and optional Boolean keep-state; all 13 result/context/trace fields and independent expected values must match. Random source inputs are independent per runtime. Commented historical numeric options stay inactive. Full production API and node parity remain unverified.')
    (build / 'original-verify-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(dict(passed=report['passed'],original_methods=6,original_assertions_per_runtime=56,
                         classes=[dict(name=r['name'],passed=r['passed']) for r in results],
                         supplemental_comparisons=len(expressions),independent_checks=checks,failures=failures),indent=2))
    return not report['passed']

if __name__ == '__main__':
    raise SystemExit(main())
