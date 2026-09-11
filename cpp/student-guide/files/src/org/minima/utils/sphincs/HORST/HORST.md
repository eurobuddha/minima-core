# HORST — teaching guide

**Teaching status:** Authored explanation of the pinned source, native translation, and concrete validation. Full parity remains unverified.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/HORST.java) · [Native counterpart](../../../../../../../../src/org/minima/utils/sphincs/HORST/HORST.cpp)

## Purpose

HORST reveals selected secret preimages and accompanies each with an MMR proof under one public-key root. A 32-byte message hash selects 16 indices, each represented by two adjacent bytes.

## Follow the algorithm

1. Construction derives a PrivateKey and then builds PublicKey from it. The HORST root is the public key’s MMR root.
2. signMessage hashes the raw message bytes. For positions 0 through 15, getKeyRef copies two bytes at offsets 2*i and 2*i+1 and interprets them through MiniData as an unsigned integer.
3. For each index, append that secret preimage and the matching public-tree proof to parallel signature lists. Repeated two-byte chunks may select the same index more than once.
4. Verification hashes the message again, recomputes each index, hashes the supplied preimage, reconstructs an MMR leaf with that index, and calculates the supplied proof. Any root mismatch returns false; all 16 matches return true.
5. The method assumes the required list entries exist. It does not first validate list lengths, so a truncated signature may throw rather than simply return false. Extra entries after the 16 used values are ignored by the verification loop.

## How the C++ translation works

The native HORST builds the real PrivateKey and PublicKey objects, then reuses raw Crypto hashing and the existing indexed MMR leaf/proof functions. getKeyRef performs the source's 32-bit wraparound with unsigned arithmetic and checks each byte access in order, avoiding undefined signed overflow. Verification performs sixteen sequential checks without prevalidating list sizes; a root mismatch can return false before a later missing or null entry is reached. The source log and main demonstration are also translated.

## Understanding the tests

Java and C++ sign fixed, empty, long, and repeated-index messages using three full-size trees. Every signature is supplied to both verifiers, including the opposite writer, with explicit expected success. Python separately checks the selected secrets and reconstructs all authentication paths. Negative checks alter each secret slot, messages, roots, sums, proof bytes, and list shapes. Null combinations check failure order; index tests include Java int multiplication wraparound. The actual Java and native main demonstrations must produce identical output ending in Verify : true.

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

If two chunks of the message hash are identical, how many signature list entries are still appended and which secret values do they contain?

Pinned Java SHA-256: `320f0ac46e9e2a0ab9556ee0fa3f69bbe8006890a8b8b6d12d8465d3a0eda904`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
