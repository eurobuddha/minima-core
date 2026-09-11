#!/usr/bin/env python3
"""Pinned AesUtil: exact bytes, nullable UTF-16, helper errors and cipher reuse."""
import argparse
import base64
import hashlib
import itertools
import json
import os
from pathlib import Path
import random
import subprocess

ROOT = Path(__file__).resolve().parents[1]
JAVA = ROOT.parent / 'upstream-study'


def units(text):
    if text is None or isinstance(text, list):
        return text
    raw = text.encode('utf-16-be', 'surrogatepass')
    return [int.from_bytes(raw[i:i+2], 'big') for i in range(0, len(raw), 2)]


def from_units(value):
    return b''.join(c.to_bytes(2, 'big') for c in value).decode('utf-16-be', 'surrogatepass')


def text_result(text):
    return ['text', ''.join(f'{c:04x}' for c in units(text))]


def hx(raw):
    return '0x' + raw.hex().upper() if raw else ''


SALT = '4452150bad3b58e9d2c043ad24db2b1d'
IV = '0d93cefd0147ecb48a379b5be0da7e8a'
PASSWORD = 'apasswordblabla'


def operation(name, data=None, salt=SALT, iv=IV, password=PASSWORD, **extra):
    return dict(op=name, data=units(data), salt=units(salt), iv=units(iv), password=units(password), **extra)


def program(ops, bits=128, iterations=1000):
    return dict(bits=bits, iterations=iterations, ops=ops)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--build', type=Path, default=ROOT / 'build')
    args = ap.parse_args()
    build = args.build.resolve()
    oracle = build / 'aes-util-oracle'
    oracle.mkdir(exist_ok=True)
    sources = sorted((JAVA / 'src').rglob('*.java')) + [ROOT / 'support/AesUtilOracle.java']
    listing = build / 'aes-util-java-sources.txt'
    listing.write_text('\n'.join('"' + str(p) + '"' for p in sources) + '\n')
    subprocess.run(['javac', '-cp', str(JAVA / 'jar/minima.jar'), '-d', str(oracle), '@' + str(listing)], check=True)
    java = ['java', '-Dfile.encoding=UTF-8', '-cp', f'{oracle}:{JAVA}/jar/minima.jar', 'AesUtilOracle']
    cpp = [str(build / 'minima_aes_util')]
    env = {**os.environ, 'TZ': 'UTC', 'UBSAN_OPTIONS': 'halt_on_error=1'}

    def run(command, cases):
        p = subprocess.run(command, input='\n'.join(json.dumps(c) for c in cases) + '\n',
                           capture_output=True, text=True, timeout=120, env=env)
        if p.returncode or len(p.stdout.splitlines()) != len(cases):
            (build / 'aes-util-crash-stderr.txt').write_text(p.stderr)
            raise RuntimeError(f'Process {p.returncode}; {len(p.stdout.splitlines())} outputs for {len(cases)} inputs')
        return [json.loads(line) for line in p.stdout.splitlines()]

    rng = random.Random(5564)
    cases = []
    independent = []
    texts = ['', 'helloyou!', 'é 🐼\x00 π', 'a\r\nb\t', [0xd800], [0xdc00], [0xd800, 0xd800, 0xdc00, 0xdc00]]
    texts += ['a' * n for n in [1, 15, 16, 17, 31, 32, 33, 255, 256, 257, 4096, 16384]]
    texts += [[rng.randrange(65536) for _ in range(rng.randrange(100))] for _ in range(35)]
    # Exact key bytes also have an independent hashlib oracle. Java and native
    # are not allowed to agree on a broken derivation or two null round trips.
    for bits in [-1, 0, 1, 7, 8, 15, 120, 127, *range(128, 137), 160, *range(192, 201), *range(256, 265), 320]:
        for iterations in [-1, 0, 1, 2, 1000]:
            password = rng.choice(['', PASSWORD, 'é🐼\x00', [0xd800, 0xdc00, 0xd800]])
            ops = [operation('derive', password=password), operation('encrypt', 'helloyou!', password=password)]
            index = len(cases)
            cases.append(program(ops, bits, iterations))
            if bits >= 8 and iterations > 0:
                encoded = from_units(units(password)).encode('utf-8', 'replace')
                key = hashlib.pbkdf2_hmac('sha1', encoded, bytes.fromhex(SALT), iterations, bits // 8)
                independent.append((index, 0, ['bytes', hx(key)]))
    for bits, iterations in itertools.product([128, 192, 256], [4096, 65536]):
        cases.append(program([operation('derive'), operation('encrypt', 'slower derivation')], bits, iterations))
    for bits in [128, 192, 256]:
        for value in texts:
            cases.append(program([operation('encrypt', value), operation('decrypt', '!'), operation('encrypt', value)], bits))
    # Failure order, empty salt, odd hex, prefix/case, and each nullable input.
    for name in ['encrypt', 'decrypt', 'derive']:
        for field, values in [('salt', [None, '', '0x', 'f', '0XAbCd', '0xﬀ', ' x ', '0xgg']),
                              ('iv', [None, '', '0x', 'ff', '00' * 15, '00' * 17, 'zz']),
                              ('password', [None, '', [0xd800], [0xdc00], '🐼\x00é']),
                              ('data', [None, '', 'AA', 'AAAA', 'AAAA=', 'AAAA===='])]:
            for value in values:
                op = operation(name, 'helloyou!' if name == 'encrypt' else '')
                op[field] = units(value)
                cases.append(program([op, operation('encrypt', 'recovered')]))
    for text in [None, '', '0x', 'f', 'abc', '0Xabcd', '0xff', '0xﬀ', '0x１２', '0x\n01', '-1', 'GG', 'π', '🐼']:
        cases.append(program([operation('hex', text)]))
    codec_ops = []
    for length in range(5):
        for chars in itertools.product('Az/=!', repeat=length):
            codec_ops.append(operation('decode', ''.join(chars)))
    for value in [None, 'éé', 'ÿÿ', 'ππ', '🐼', [0xd800], 'AA\n', 'AA ', 'AA\t', 'AA-_', 'AAAA=AAAA', 'AA==AA']:
        codec_ops.append(operation('decode', value))
    for n in [0, 1, 2, 3, 15, 16, 17, 255, 256, 1024]:
        raw = rng.randbytes(n)
        codec_ops.append(dict(op='encode', data=hx(raw)))
        for encoded in [base64.b64encode(raw).decode(), base64.b64encode(raw).decode().rstrip('=')]:
            codec_ops.append(operation('decode', encoded))
    codec_ops += [dict(op='encode', data=None), dict(op='fail')]
    cases.append(program(codec_ops))
    # Observe the private helper's caught checked exceptions versus uncaught
    # argument errors. Real callers always use encrypt/decrypt modes 1 and 2.
    for mode, key, iv, data in itertools.product([1, 2], [None, '00', '00' * 16, '00' * 24, '00' * 32],
                                                [None, '', IV, 'gg', '00' * 15], [None, '', '0x00', '0x' + '00' * 16]):
        bad = dict(op='final', mode=mode, key=key, iv=units(iv), data=data)
        cases.append(program([bad, operation('encrypt', 'recovered'), operation('encrypt', 'recovered')]))
        cases.append(program([operation('encrypt', 'select provider'), bad, operation('encrypt', 'recovered')]))

    expected, actual = run(java, cases), run(cpp, cases)
    initial_count = len(cases)
    derived = []
    recovery = []
    for original, outputs in zip(cases, expected):
        for op, result in zip(original['ops'], outputs):
            if op['op'] != 'encrypt' or result[0] != 'text':
                continue
            cipher = bytes.fromhex(result[1]).decode('utf-16-be')
            raw = base64.b64decode(cipher)
            variants = [cipher, cipher.rstrip('='), base64.b64encode(raw[:-1]).decode(),
                        base64.b64encode(raw + b'\x00').decode(), base64.b64encode(raw[:-1] + bytes([raw[-1] ^ 1])).decode()]
            ops = [{**op, 'op': 'decrypt', 'data': units(c)} for c in variants]
            ops += [{**op, 'op': 'decrypt', 'data': units(cipher), 'password': units('wrong password')}, op]
            index = len(derived)
            derived.append(program(ops, original['bits'], original['iterations']))
            plain = from_units(op['data']).encode('utf-8', 'replace').decode('utf-8')
            recovery += [(initial_count + index, 0, text_result(plain)), (initial_count + index, 1, text_result(plain)),
                         (initial_count + index, 6, result)]
    # Encrypt raw invalid UTF-8 with actual primitives in each runtime, then
    # feed the resulting ciphertext through the public decoder in the other.
    raw_inputs = [b'', b'\xff', b'\xed\xa0\x80', b'\xe2\x82', b'\xf0\x80\x80\x80', bytes(range(256)), b'a\x00b']
    raw_cases = [program([{**operation('raw'), 'data': hx(raw)}]) for raw in raw_inputs]
    raw_java, raw_cpp = run(java, raw_cases), run(cpp, raw_cases)
    for outputs in [raw_java, raw_cpp]:
        for result in outputs:
            assert result[0][0] == 'text', result
            cipher = bytes.fromhex(result[0][1]).decode('utf-16-be')
            derived.append(program([operation('decrypt', cipher), operation('encrypt', 'recovered')]))
    # Ciphertexts are deterministic, so equality also proves the initial
    # decryption corpus contains the bytes produced by the native encryptor.
    more_java, more_cpp = run(java, derived), run(cpp, derived)
    cases += derived + raw_cases
    expected += more_java + raw_java
    actual += more_cpp + raw_cpp
    failures = [{'index': i, 'case': c, 'java': j, 'cpp': n} for i, (c, j, n) in enumerate(zip(cases, expected, actual)) if j != n]
    for index, op_index, want in independent + recovery:
        if expected[index][op_index] != want or actual[index][op_index] != want:
            failures.append(dict(index=index, operation=op_index, expected=want, java=expected[index][op_index], cpp=actual[index][op_index]))
    demos = [subprocess.run(cmd + ['--demo'], capture_output=True, text=True, check=True, env=env, timeout=20).stdout for cmd in [java, cpp]]
    if demos[0] != demos[1] or len(demos[0].splitlines()) != 2 or demos[0].splitlines()[1] != 'helloyou!':
        failures.append(dict(demos=demos))
    report = dict(programs=len(cases), operations=sum(len(c['ops']) for c in cases), independent_key_checks=len(independent),
                  cross_decryption_and_recovery_checks=len(recovery), demonstration=demos[0].splitlines(),
                  corpus_sha256=hashlib.sha256(json.dumps(cases, sort_keys=True).encode()).hexdigest(), failures=failures,
                  scope='UTF-8 default charset; nullable UTF-16 public APIs, private derivation/final/fail, exact codecs and error categories/messages except NPE prose, retained cipher failure recovery and opposite-runtime ciphertexts. No non-UTF-8 default charset, provider substitution, arbitrary modes, allocation exhaustion or concurrent cipher access certification.')
    (build / 'aes-util-differential-results.json').write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps({**report, 'failures': failures[:5]}, indent=2))
    return bool(failures)


if __name__ == '__main__':
    raise SystemExit(main())
