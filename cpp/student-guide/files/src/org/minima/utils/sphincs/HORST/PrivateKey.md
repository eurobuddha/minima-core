# PrivateKey — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/PrivateKey.java) · `src/org/minima/utils/sphincs/HORST/PrivateKey.cpp` (absent)

## Purpose

This private key deterministically expands one seed into 65536 secret values. Each index selects a secret preimage later revealed by a HORST signature.

## Follow the algorithm

1. Compute the array length as 2^16 and allocate that many MiniData slots.
2. For each index i, hash the serialization of the seed followed by new MiniNumber(i) through Crypto.hashAllObjects. This hashes structured bytes, not the seed followed by a machine-endian integer.
3. Store each derived MiniData in the indexed array. Reusing the same seed and serialization produces the same full array.
4. getSize returns array length; getKey returns the stored reference directly. Negative or excessive indices fail through Java array bounds.

## Translation work still required

The mapped native counterpart is absent. These notes explain the pinned Java construction and identify dependencies to reuse; they do not claim a native implementation or a standardized SPHINCS implementation. The repository-specific serialization, MMR sum values, and seed derivation must be preserved exactly. Existing Crypto, MiniNumber, and MiniData routines are the relevant lower-level building blocks; replacing them with a generic key generator would change every public leaf.

## Understanding the tests

The HORST main creates a key from a fixed seed and prints its size/root before signing and verifying. It does not assert individual derived bytes. A future fixture should compare keys at indices 0, 1, 255, 256, and 65535 against Java, assert size 65536, repeat construction for determinism, and distinguish raw-byte hashing from serialized-object hashing.

- [Java demonstration and key caller](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/HORST.java)

This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

No native parity test for this absent counterpart is claimed. Printed Java demonstrations are not assertion-based tests, and agreement on a round trip would not by itself establish signature security. The proposed tests remain future work.

## Check your understanding

Why does changing MiniNumber serialization change all derived keys even though the mathematical index values stay the same?

Pinned Java SHA-256: `a660c967225d2a09802325fceb90aff5f8fee1db497aba2b1806b5c4006c1eb4`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
