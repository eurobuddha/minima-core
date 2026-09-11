#!/usr/bin/env python3
"""Run every active pinned transaction getter JUnit method and its native counterpart."""
import argparse
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'
TESTS = {'GETINADDRTests': 15, 'GETINAMTTests': 16, 'GETINIDTests': 15, 'GETINTOKTests': 15, 'GETOUTADDRTests': 15, 'GETOUTAMTTests': 16, 'GETOUTTOKTests': 15}

def execute(command, data=None):
    result = subprocess.run(command, input=data, capture_output=True, text=True, timeout=60)
    return dict(command=command, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'build')
    build = parser.parse_args().build.resolve()
    oracle = build / 'original-getter-oracle'
    oracle.mkdir(exist_ok=True)
    package = JAVA / 'test/org/minima/tests/kissvm/functions/txn'
    sources = sorted((JAVA / 'src').rglob('*.java')) + [package / ('input' if name.startswith('GETIN') else 'output') / (name + '.java') for name in TESTS]
    sources += [ROOT / 'support/OriginalGetterTests.java', ROOT / 'support/KissVMOracle.java']
    listing = oracle / 'sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    dependencies = f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac', '-cp', dependencies, '-d', str(oracle), '@' + str(listing)], check=True)
    java = ['java', '-cp', f'{oracle}:{dependencies}']
    results = []
    for name, assertions in TESTS.items():
        expected = dict(passed=True, tests=3, assertions=assertions, **{'class': name})
        outputs = [execute(java + ['OriginalGetterTests', name]), execute([str(build / name)])]
        passed = all(p['returncode'] == 0 and not p['stderr'] for p in outputs)
        for output in outputs:
            try:
                lines = output['stdout'].splitlines()
                if len(lines) != 1 or not lines[0].startswith('ORIGINAL_GETTER_RESULT '):
                    raise ValueError('Unexpected test output')
                passed = (json.loads(lines[0].removeprefix('ORIGINAL_GETTER_RESULT ')) == expected) and passed
            except ValueError:
                passed = False
        results.append(dict(name=name, passed=passed, outputs=outputs))

    # Reuse the rich KISS fixture: distinct witness coin IDs and non-Minima
    # token units make observations absent from the original all-Minima getters.
    expressions=[]; expected=[]
    for name in TESTS:
        fn=name.removesuffix('Tests')
        for index in range(2 if fn.endswith('AMT') else 3):
            expression=f'{fn}({index})'
            if fn.endswith('ADDR'): value=[1,f'0x2{index}']
            elif fn=='GETINID': value=[1,f'0xA{index}']
            elif fn.endswith('AMT'): value=[2,(['7','25'] if fn=='GETINAMT' else ['5','12.5'])[index]]
            elif index==1: expression+=' EQ tok';value=[8,'TRUE']
            else: value=[1,'0x00' if index==0 else '0x77']
            expressions.append(expression);expected.append(value)
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
    report = dict(passed=all(r['passed'] for r in results) and supplemental['passed'], original_methods=21,
                  original_assertions_per_runtime=107, results=results,supplemental=supplemental,
                  scope='All seven original getter test files, 21 annotated methods and newSimpleAddress helpers, with exact active fixtures, random sizes and assertions. Native shared Coin handles preserve source token metadata clearing after transaction insertion; valid/invalid amount fixture differences retained. GETINID valid fixture retains both witness proofs, invalid fixture has none. Additional rich KISS fixture checks distinct witness IDs, non-Minima token IDs and scaled amounts; exact result/context/trace comparisons and known values required. Random source inputs independent per runtime. Full getter API, malformed input, mutation/concurrency and node parity not certified.')
    (build / 'original-getter-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(dict(passed=report['passed'],original_methods=21,original_assertions_per_runtime=107,
                         classes=[dict(name=r['name'],passed=r['passed']) for r in results],
                         supplemental_comparisons=len(expressions),independent_checks=checks,failures=failures),indent=2))
    return not report['passed']

if __name__ == '__main__':
    raise SystemExit(main())
