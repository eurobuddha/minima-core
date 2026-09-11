#!/usr/bin/env python3
"""Run every active pinned statement command JUnit method and its native counterpart."""
import argparse
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'
TESTS = {'TokenTests': 699}

def execute(command, data=None):
    result = subprocess.run(command, input=data, capture_output=True, text=True, timeout=60)
    return dict(command=command, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'build')
    build = parser.parse_args().build.resolve()
    oracle = build / 'original-tokens-oracle'
    oracle.mkdir(exist_ok=True)
    package = JAVA / 'test/org/minima/tests/kissvm/tokens'
    sources = sorted((JAVA / 'src').rglob('*.java')) + [package / (name + '.java') for name in TESTS]
    sources += [ROOT / 'support/OriginalTokenTests.java', ROOT / 'support/KissOracle.java']
    listing = oracle / 'sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    dependencies = f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac', '-cp', dependencies, '-d', str(oracle), '@' + str(listing)], check=True)
    java = ['java', '-cp', f'{oracle}:{dependencies}']
    results = []
    for name, assertions in TESTS.items():
        expected = dict(passed=True, tests=4, assertions=assertions, **{'class': name})
        outputs = [execute(java + ['OriginalTokenTests', name]), execute([str(build / 'KissTokenTests')])]
        passed = all(p['returncode'] == 0 and not p['stderr'] for p in outputs)
        for output in outputs:
            try:
                lines = output['stdout'].splitlines()
                if len(lines) != 1 or not lines[0].startswith('ORIGINAL_TOKENS_RESULT '):
                    raise ValueError('Unexpected test output')
                passed = (json.loads(lines[0].removeprefix('ORIGINAL_TOKENS_RESULT ')) == expected) and passed
            except ValueError:
                passed = False
        results.append(dict(name=name, passed=passed, outputs=outputs))

    # Fixed parser-visible spelling and token metadata through existing probes.
    cases=[('[ abc [ def ] ghi ]',[[3,'[ abc [ def ] ghi ]','VALUE:[ abc [ def ] ghi ]']]),
           ('0x1 0xab 0x00AB',[[3,t,'VALUE:'+t] for t in ['0x1','0xab','0x00AB']]),
           ('TRUE FALSE ( )',[[9,'TRUE','TRUE:TRUE'],[10,'FALSE','FALSE:FALSE'],[7,'(','OPENBRACKET:('],[8,')','CLOSEBRACKET:)']]),
           ('a'*32,[[4,'a'*32,'VARIABLE:'+'a'*32]])]
    data=''.join(f'tokens 0x{script.encode().hex()} 0\n' for script,_ in cases)
    observations=[];failures=[];checks=0
    for runtime,command in [('java',java+['KissOracle']),('cpp',[str(build/'minima_kiss_values')])]:
        output=execute(command,data)
        if output['returncode'] or output['stderr']:raise RuntimeError(output)
        rows=[json.loads(bytes.fromhex(line.removeprefix('0x')).decode()) for line in output['stdout'].splitlines()]
        if len(rows)!=len(cases):raise RuntimeError('Probe count mismatch')
        observations.append(rows)
        for i,(row,(_,expected)) in enumerate(zip(rows,cases)):
            checks+=1
            if row!=expected:failures.append(dict(runtime=runtime,case=i,expected=expected,actual=row))
    for i,(j,n) in enumerate(zip(*observations)):
        if j!=n:failures.append(dict(case=i,java=j,cpp=n))
    supplemental=dict(passed=not failures,comparisons=len(cases),independent_checks=checks,
                      cases=cases,observations=observations,failures=failures)
    report = dict(passed=all(r['passed'] for r in results) and supplemental['passed'], original_methods=4,
                  original_assertions_per_runtime=699, results=results,supplemental=supplemental,
                  scope='All four annotated TokenTests methods and 699 source assertions, full registry loops, exact source token lists and five random sizes 4/8/16/32/64 bytes retained. Source comments questioning variable length and numeric syntax do not replace active expectations. Supplemental fixed tokens compare exact type, spelling and display metadata for nested strings, hex preservation, brackets, Booleans and 32-character variables. Full token/registry mutation and node parity remain unverified.')
    (build / 'original-tokens-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(dict(passed=report['passed'],original_methods=4,original_assertions_per_runtime=699,
                         classes=[dict(name=r['name'],passed=r['passed']) for r in results],
                         supplemental_comparisons=len(cases),independent_checks=checks,failures=failures),indent=2))
    return not report['passed']

if __name__ == '__main__':
    raise SystemExit(main())
