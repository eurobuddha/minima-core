#!/usr/bin/env python3
"""Run every active pinned statement command JUnit method and its native counterpart."""
import argparse
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'
TESTS = {'ExpressionParserTests': {'side': 'expressions', 'assertions': 1254, 'parse_calls': 654, 'methods': ['testBooleanExpressions', 'testBooleanExpressionsUnary', 'testOperatorExpressions', 'testOperatorExpressionsUnary', 'testOperatorExpressionsExceptions']}, 'StatementParserTests': {'side': 'statements', 'assertions': 15, 'parse_calls': 93, 'methods': ['testConstructors', 'testParsingLETArrays', 'testParsingLET', 'testParsingEXEC', 'testParsingMAST', 'testParsingIF', 'testParsingWHILE', 'testParsingASSERT', 'testParsingRETURN', 'testParsingException']}}

def execute(command, data=None):
    result = subprocess.run(command, input=data, capture_output=True, text=True, timeout=60)
    return dict(command=command, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'build')
    build = parser.parse_args().build.resolve()
    oracle = build / 'original-parsers-oracle'
    oracle.mkdir(exist_ok=True)
    package = JAVA / 'test/org/minima/tests/kissvm'
    sources = sorted((JAVA / 'src').rglob('*.java')) + [package / spec['side'] / (name + '.java') for name,spec in TESTS.items()]
    sources += [ROOT / 'support/OriginalParserTests.java', ROOT / 'support/KissVMOracle.java']
    listing = oracle / 'sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    dependencies = f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac', '-cp', dependencies, '-d', str(oracle), '@' + str(listing)], check=True)
    java = ['java', '-cp', f'{oracle}:{dependencies}']
    results = []
    for name, spec in TESTS.items():
        expected = dict(passed=True, tests=len(spec['methods']), assertions=spec['assertions'], parse_calls=spec['parse_calls'], **{'class': name})
        outputs = [execute(java + ['OriginalParserTests', name]), execute([str(build / name)])]
        passed = all(p['returncode'] == 0 and not p['stderr'] for p in outputs)
        for output in outputs:
            try:
                lines = output['stdout'].splitlines()
                if len(lines) != 1 or not lines[0].startswith('ORIGINAL_PARSERS_RESULT '):
                    raise ValueError('Unexpected test output')
                passed = (json.loads(lines[0].removeprefix('ORIGINAL_PARSERS_RESULT ')) == expected) and passed
            except ValueError:
                passed = False
        results.append(dict(name=name, passed=passed, outputs=outputs))

    # Distinguish source parsing tests from execution and precedence checks.
    cases=[('expr','TRUE OR FALSE AND FALSE',[8,'FALSE']),
           ('expr','20 % 3 * 2',[2,'2']),
           ('expr','1+2*3',[2,'7']),
           ('expr','8/2/2',[2,'2']),
           ('expr','0x01 << 1*2',[1,'0x04']),
           ('expr','NOT TRUE EQ FALSE',[8,'TRUE']),
           ('run','LET (1 2)=42 RETURN GET(1 2) EQ 42',True),
           ('run','LET a=0 WHILE a LT 2 DO IF TRUE THEN LET a=a+1 ENDIF ENDWHILE RETURN a EQ 2',True)]
    data=''.join(f'{mode} 0x{script.encode().hex()} 1024 1\n' for mode,script,_ in cases)
    observations=[];failures=[];checks=0
    for runtime,command in [('java',java+['KissVMOracle']),('cpp',[str(build/'minima_kissvm')])]:
        output=execute(command,data)
        if output['returncode'] or output['stderr']:raise RuntimeError(output)
        rows=[json.loads(bytes.fromhex(line.removeprefix('0x')).decode()) for line in output['stdout'].splitlines()]
        if len(rows)!=len(cases):raise RuntimeError('Probe count mismatch')
        observations.append(rows)
        for i,(row,(mode,_,expected)) in enumerate(zip(rows,cases)):
            checks+=1
            good=(len(row)==13 and row[1][:2]==expected and not row[6] and not row[7]) if mode=='expr' else (len(row)==11 and row[:6]==[True,expected,True,False,'',''])
            if not good:failures.append(dict(runtime=runtime,case=i,expected=expected,actual=row))
    for i,(j,n) in enumerate(zip(*observations)):
        if j!=n:failures.append(dict(case=i,java=j,cpp=n))
    supplemental=dict(passed=not failures,comparisons=len(cases),independent_checks=checks,
                      cases=cases,observations=observations,failures=failures)
    report = dict(passed=all(r['passed'] for r in results) and supplemental['passed'], original_methods=15,
                  original_assertions_per_runtime=1269, results=results,supplemental=supplemental,
                  scope='Both complete active ExpressionParserTests and StatementParserTests: 15 annotated methods, 1269 original assertions and 747 direct parser calls per runtime. Original operator loops, duplicated inputs, cleanScript calls, handcrafted token types, exact expression classes and expected parse exceptions remain. Successful statement parse cases are smoke checks, not execution proof. Supplemental known-outcome full KISS records check precedence, arrays and nested control flow. Full parser/API/runtime parity remains unverified.')
    (build / 'original-parsers-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(dict(passed=report['passed'],original_methods=15,original_assertions_per_runtime=1269,
                         classes=[dict(name=r['name'],passed=r['passed']) for r in results],
                         supplemental_comparisons=len(cases),independent_checks=checks,failures=failures),indent=2))
    return not report['passed']

if __name__ == '__main__':
    raise SystemExit(main())
