#!/usr/bin/env python3
"""Run every active pinned statement command JUnit method and its native counterpart."""
import argparse
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'
TESTS = {'LETstatementTests': 30}

def execute(command, data=None):
    result = subprocess.run(command, input=data, capture_output=True, text=True, timeout=60)
    return dict(command=command, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'build')
    build = parser.parse_args().build.resolve()
    oracle = build / 'original-let-oracle'
    oracle.mkdir(exist_ok=True)
    package = JAVA / 'test/org/minima/tests/kissvm/statements/commands'
    sources = sorted((JAVA / 'src').rglob('*.java')) + [package / (name + '.java') for name in TESTS]
    sources += [ROOT / 'support/OriginalLETTests.java', ROOT / 'support/KissVMOracle.java']
    listing = oracle / 'sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    dependencies = f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac', '-cp', dependencies, '-d', str(oracle), '@' + str(listing)], check=True)
    java = ['java', '-cp', f'{oracle}:{dependencies}']
    results = []
    for name, assertions in TESTS.items():
        expected = dict(passed=True, tests=2, assertions=assertions, **{'class': name})
        outputs = [execute(java + ['OriginalLETTests', name]), execute([str(build / name)])]
        passed = all(p['returncode'] == 0 and not p['stderr'] for p in outputs)
        for output in outputs:
            try:
                lines = output['stdout'].splitlines()
                if len(lines) != 1 or not lines[0].startswith('ORIGINAL_LET_RESULT '):
                    raise ValueError('Unexpected test output')
                passed = (json.loads(lines[0].removeprefix('ORIGINAL_LET_RESULT ')) == expected) and passed
            except ValueError:
                passed = False
        results.append(dict(name=name, passed=passed, outputs=outputs))

    # Original array execution examples are inactive; retain them as comments
    # and independently exercise numeric array keys through the real parser/GET.
    cases=[
        ('LET (1 2)=42 RETURN GET(1 2) EQ 42',True,True,None),
        ('LET (-1 0.5)=7 RETURN GET(-1 0.5) EQ 7',True,True,None),
        ('LET (1 2)=7 LET (1 2)=9 RETURN GET(1 2) EQ 9',True,True,None),
        ('LET x=1 LET (x+1 3)=11 RETURN GET(2 3) EQ 11',True,True,None),
    ]
    data=''.join(f'run 0x{script.encode().hex()} 1024 1\n' for script,*_ in cases)
    observations=[];failures=[];checks=0
    for runtime,command in [('java',java+['KissVMOracle']),('cpp',[str(build/'minima_kissvm')])]:
        output=execute(command,data)
        if output['returncode'] or output['stderr']:raise RuntimeError(output)
        rows=[json.loads(bytes.fromhex(line.removeprefix('0x')).decode()) for line in output['stdout'].splitlines()]
        if len(rows)!=len(cases):raise RuntimeError('Probe count mismatch')
        observations.append(rows)
        for i,(row,(_,success_set,success,a)) in enumerate(zip(rows,cases)):
            checks+=1
            if len(row)!=11 or row[:6]!=[True,success,success_set,False,'',''] or (a is not None and row[9].get('a')!=a):
                failures.append(dict(runtime=runtime,case=i,expected=[success_set,success,a],actual=row))
    for i,(j,n) in enumerate(zip(*observations)):
        if j!=n:failures.append(dict(case=i,java=j,cpp=n))
    supplemental=dict(passed=not failures,comparisons=len(cases),independent_checks=checks,
                      cases=cases,observations=observations,failures=failures)
    report = dict(passed=all(r['passed'] for r in results) and supplemental['passed'], original_methods=2,
                  original_assertions_per_runtime=30, results=results,supplemental=supplemental,
                  scope='Original LETstatementTests retains both annotated methods, all 20 constructor and 10 execution assertions. Constructor-only function/global/variable and heterogeneous array fixtures are not claimed as executed expressions; commented source execution cases remain inactive. Four supplemental numeric-array scripts check multidimensional, negative/fractional, overwritten and evaluated keys through LET and GET, with complete 11-field Java/native records and independent success expectations. Full LET API and node parity remain unverified.')
    (build / 'original-let-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(dict(passed=report['passed'],original_methods=2,original_assertions_per_runtime=30,
                         classes=[dict(name=r['name'],passed=r['passed']) for r in results],
                         supplemental_comparisons=len(cases),independent_checks=checks,failures=failures),indent=2))
    return not report['passed']

if __name__ == '__main__':
    raise SystemExit(main())
