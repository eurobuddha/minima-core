# PrivateKey — teaching guide

**Teaching status:** Authored explanation of the pinned source, native translation, and concrete validation. Full parity remains unverified.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/PrivateKey.java) · [Native counterpart](../../../../../../../../src/org/minima/utils/sphincs/HORST/PrivateKey.cpp)

## Purpose

This private key deterministically expands one seed into 65536 secret values. Each index selects a secret preimage later revealed by a HORST signature.

## Follow the algorithm

1. Compute the array length as 2^16 and allocate that many MiniData slots.
2. For each index i, hash the serialization of the seed followed by new MiniNumber(i) through Crypto.hashAllObjects. This hashes structured bytes, not the seed followed by a machine-endian integer.
3. Store each derived MiniData in the indexed array. Reusing the same seed and serialization produces the same full array.
4. getSize returns array length; getKey returns the stored reference directly. Negative or excessive indices fail through Java array bounds.

## How the C++ translation works

The C++ class reuses Crypto.hashAllObjects with the existing MiniData and MiniNumber serialization. The fixed 65,536 slots hold shared MiniData pointers so signatures retain the selected secret object. Bounds checks throw the Java array-bounds category. For a null seed, Java Crypto catches and logs a null-pointer failure for each derivation and returns null each time; the native constructor preserves all 65,536 iterations and null slots.

## Understanding the tests

Three complete seed expansions, including an empty seed and a 33-byte seed, are compared with Java. Python independently computes every secret using hashlib.sha3_256 and compares a digest of the concatenated 65,536 values. Selected values test byte/sign boundaries at 127/128, 255/256, 32767/32768, and the last index. Tests check invalid indices, getter identity, selected-secret identity in signatures, and all 65,536 null-seed logging events.

The [HORST lab](../../../../../../../../student-guide/new-tests/horst-signatures.md)
explains the independent reconstruction and records completed execution results.
The [test driver](../../../../../../../../tools/horst_differential.py) calls the
unmodified Java sources and the compiled native library. The source demonstration
alone contains no assertions; the driver adds explicit acceptance and rejection checks.

## Limits and a useful next test

These cases cover the specified algorithm and error boundaries, not all possible
Java subclass overrides, concurrent mutation, or mutable raw Java byte-array aliases.
Null-pointer/EOF prose and runtime-specific stack frames are normalized by the fixtures.
The Java test VM disables fast-throw diagnostic omission so repeated array errors
retain comparable messages. FORS/SPHINCS integration and full node parity remain open.
Passing these tests is not a cryptographic security certification.

## Check your understanding

Why does changing MiniNumber serialization change all derived keys even though the mathematical index values stay the same?

Pinned Java SHA-256: `a660c967225d2a09802325fceb90aff5f8fee1db497aba2b1806b5c4006c1eb4`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
