#!/usr/bin/env python3
"""Run the unchanged pinned SHA tests, then bounded independent hash/proof checks."""
import argparse
import hashlib
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'
TESTS = {'SHA2Tests': 212, 'SHA3Tests': 27, 'CHAINSHATests': 24}

def execute(command, data=None):
    result = subprocess.run(command, input=data, capture_output=True, text=True, timeout=60)
    return dict(command=command, returncode=result.returncode, stdout=result.stdout, stderr=result.stderr)

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--build', type=Path, default=ROOT / 'build')
    build = parser.parse_args().build.resolve()
    oracle = build / 'original-sha-oracle'
    oracle.mkdir(exist_ok=True)
    package = JAVA / 'test/org/minima/tests/kissvm/functions/sha'
    sources = sorted((JAVA / 'src').rglob('*.java')) + [package / (name + '.java') for name in TESTS]
    sources += [ROOT / 'support/OriginalSHATests.java', ROOT / 'support/KissVMOracle.java']
    listing = oracle / 'sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    dependencies = f'{JAVA}/jar/minima.jar:{JAVA}/lib/junit-4.13.jar'
    subprocess.run(['javac', '-cp', dependencies, '-d', str(oracle), '@' + str(listing)], check=True)
    java = ['java', '-cp', f'{oracle}:{dependencies}']
    results = []
    for name, assertions in TESTS.items():
        expected = dict(passed=True, tests=3, assertions=assertions, **{'class': name})
        outputs = [execute(java + ['OriginalSHATests', name]), execute([str(build / name)])]
        passed = all(p['returncode'] == 0 and not p['stderr'] for p in outputs)
        for output in outputs:
            try:
                lines = output['stdout'].splitlines()
                if len(lines) != 1 or not lines[0].startswith('ORIGINAL_SHA_RESULT '):
                    raise ValueError('Unexpected test output')
                passed = (json.loads(lines[0].removeprefix('ORIGINAL_SHA_RESULT ')) == expected) and passed
            except ValueError:
                passed = False
        results.append(dict(name=name, passed=passed, outputs=outputs))

    # Reuse the existing KISS expression/context/trace probes. These cases are
    # supplementary; they do not inflate the original JUnit assertion count.
    scripts = []; values = []
    for name, digest in [('SHA2', hashlib.sha256), ('SHA3', hashlib.sha3_256)]:
        for length in [0, 1, 31, 32, 55, 56, 63, 64, 65, 135, 136, 137, 255, 1024, 65536]:
            data = bytes((i * 37 + 11) % 256 for i in range(length))
            literal = '0x' + data.hex() if data else 'HEX([])'
            scripts.append(f'{name}({literal})')
            values.append([1, '0x' + digest(data).hexdigest().upper()])
        for string in ['', 'abc', 'Hello World', 'a  b', '🐼', 'café']:
            scripts.append(f'{name}([{string}])')
            values.append([1, '0x' + digest(string.encode()).hexdigest().upper()])
    fixture_output = execute(java + ['KissVMOracle', '--fixtures'])
    if fixture_output['returncode'] or fixture_output['stderr']:
        raise RuntimeError(fixture_output)
    fixtures = json.loads(bytes.fromhex(fixture_output['stdout'].strip().removeprefix('0x')).decode())
    proof_scripts = [script for script in fixtures if script.startswith('PROOF(')]
    if len(proof_scripts) != 4:
        raise RuntimeError('Proof fixture count changed')
    scripts += proof_scripts
    values += [[8, 'TRUE'], [8, 'FALSE'], [8, 'TRUE'], [8, 'FALSE']]
    data = ''.join(f'expr 0x{script.encode().hex()} 1024 0\n' for script in scripts)
    outputs = [execute(java + ['KissVMOracle'], data), execute([str(build / 'minima_kissvm')], data)]
    observations = []; failures = []; expected_checks = 0
    for runtime, output in zip(['java', 'cpp'], outputs):
        if output['returncode'] or output['stderr']:
            failures.append(dict(runtime=runtime, output=output));continue
        try:
            rows = [json.loads(bytes.fromhex(line.removeprefix('0x')).decode()) for line in output['stdout'].splitlines()]
            if len(rows) != len(scripts):
                raise ValueError('Output count mismatch')
            observations.append(rows)
            for index, (row, expected) in enumerate(zip(rows, values)):
                expected_checks += 1
                if len(row) != 13 or row[1] != expected or row[6] or row[7]:
                    failures.append(dict(runtime=runtime, case=index, expected_value=expected, actual=row))
        except (ValueError, IndexError) as error:
            failures.append(dict(runtime=runtime, error=str(error)))
    if len(observations) == 2:
        for index, (j, n) in enumerate(zip(*observations)):
            if j != n:failures.append(dict(case=index, java=j, cpp=n))
    else:
        failures.append(dict(error='Missing runtime transcript'))
    supplemental = dict(cases=len(scripts), independent_value_checks=expected_checks,
                        passed=not failures, failures=failures, scripts=scripts,
                        transcript_sha256=hashlib.sha256(json.dumps(observations, sort_keys=True).encode()).hexdigest(),
                        outputs=outputs)
    report = dict(passed=all(r['passed'] for r in results) and supplemental['passed'],
                  original_methods=9, original_assertions_per_runtime=263, results=results,
                  supplemental=supplemental,
                  scope='All active original methods and assertions, including 100 random SHA2 trials per runtime. Random inputs are independently generated, not a shared differential corpus. SHA3 valid body has only setup; CHAINSHATests valid body is commented out. All 20 original PROOF rejections are arity checks. Supplementary fixed SHA2/SHA3 raw byte and UTF-8 inputs have independent hashlib expectations; four existing Java-generated proof fixtures exercise valid/wrong sums for hex/script leaves. Exact probe traces compared for these successful evaluations; no generic error normalization is relied on. Full hashing/proof API and node parity not certified.')
    (build / 'original-sha-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(dict(passed=report['passed'], original_methods=9, original_assertions_per_runtime=263,
                         classes=[dict(name=r['name'], passed=r['passed']) for r in results],
                         supplemental_cases=len(scripts), independent_value_checks=expected_checks,
                         failures=failures, transcript_sha256=supplemental['transcript_sha256']), indent=2))
    return not report['passed']

if __name__ == '__main__':
    raise SystemExit(main())
