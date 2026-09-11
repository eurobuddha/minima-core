#!/usr/bin/env python3
"""Run every active pinned statement command JUnit method and its native counterpart."""
import argparse
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'
TESTS = {'RETURNstatementTests': 42, 'WHILEstatementTests': 1, 'IFstatementTests': 22, 'MASTstatementTests': 19}

def execute(command, data=None):
    result = subprocess.run(command, input=data, capture_output=True, text=True, timeout=60)
    return dict(command=command, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'build')
    build = parser.parse_args().build.resolve()
    oracle = build / 'original-statement-commands-oracle'
    oracle.mkdir(exist_ok=True)
    package = JAVA / 'test/org/minima/tests/kissvm/statements/commands'
    sources = sorted((JAVA / 'src').rglob('*.java')) + [package / (name + '.java') for name in TESTS]
    sources += [ROOT / 'support/OriginalStatementCommandsTests.java', ROOT / 'support/KissVMOracle.java']
    listing = oracle / 'sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    dependencies = f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac', '-cp', dependencies, '-d', str(oracle), '@' + str(listing)], check=True)
    java = ['java', '-cp', f'{oracle}:{dependencies}']
    results = []
    for name, assertions in TESTS.items():
        expected = dict(passed=True, tests=2, assertions=assertions, **{'class': name})
        outputs = [execute(java + ['OriginalStatementCommandsTests', name]), execute([str(build / name)])]
        passed = all(p['returncode'] == 0 and not p['stderr'] for p in outputs)
        for output in outputs:
            try:
                lines = output['stdout'].splitlines()
                if len(lines) != 1 or not lines[0].startswith('ORIGINAL_STATEMENTS_RESULT '):
                    raise ValueError('Unexpected test output')
                passed = (json.loads(lines[0].removeprefix('ORIGINAL_STATEMENTS_RESULT ')) == expected) and passed
            except ValueError:
                passed = False
        results.append(dict(name=name, passed=passed, outputs=outputs))

    # Full run records supplement outcome-free WHILE source cases and direct calls.
    cases=[
        ('LET a=0 WHILE a LT 5 DO LET a=a+1 ENDWHILE',False,False,'5'),
        ('LET a=0 WHILE a LT 5 DO LET a=a+1 RETURN TRUE ENDWHILE',True,True,'1'),
        ('LET a=0 WHILE FALSE DO LET a=99 ENDWHILE RETURN a EQ 0',True,True,'0'),
        ('LET a=0 WHILE a LT 5 DO LET a=a+1 RETURN FALSE ENDWHILE RETURN TRUE',True,False,'1'),
        ('IF FALSE THEN RETURN TRUE ELSEIF TRUE THEN RETURN FALSE ELSE RETURN TRUE ENDIF',True,False,None),
        ('IF FALSE THEN RETURN FALSE ELSE RETURN TRUE ENDIF',True,True,None),
        ('RETURN TRUE RETURN FALSE',True,True,None),
        ('RETURN FALSE RETURN TRUE',True,False,None),
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
    report = dict(passed=all(r['passed'] for r in results) and supplemental['passed'], original_methods=8,
                  original_assertions_per_runtime=84, results=results,supplemental=supplemental,
                  scope='All four original RETURN, WHILE, IF and MAST command test files, eight annotated methods and 84 assertions per runtime retain their complete active fixtures. WHILE execution has no source outcome assertions; supplemental fixed scripts independently check final loop state and early return. Complete 11-field Java/native run records must match. Source constructor mistakes and inactive comments are retained as source behavior. Full production API and node parity remain unverified.')
    (build / 'original-statement-commands-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(dict(passed=report['passed'],original_methods=8,original_assertions_per_runtime=84,
                         classes=[dict(name=r['name'],passed=r['passed']) for r in results],
                         supplemental_comparisons=len(cases),independent_checks=checks,failures=failures),indent=2))
    return not report['passed']

if __name__ == '__main__':
    raise SystemExit(main())
