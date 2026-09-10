# SPHINCS — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCS.java) · `src/org/minima/utils/sphincs/SPHINCS.cpp` (absent)

## Purpose

This repository-specific construction joins a Minima TreeKey signature to a FORS signature. The TreeKey authenticates the FORS root, and FORS authenticates the message. The class name alone does not establish equivalence to a standardized SPHINCS scheme.

## Follow the algorithm

1. initSeed hashes the original seed concatenated with WOTS and separately FORS text to derive two 32-byte seeds. It concatenates those seeds as the private key, builds a TreeKey with 192 keys per level and depth 4, and records its public root and maximum uses.
2. initPrivateKey retains the supplied private-key value, copies its first 32 and next 32 bytes into the two seeds, and derives the same TreeKey public root. Too-short data fails while extra bytes are retained in the stored private key but ignored by seed extraction.
3. signMessage hashes raw message bytes, reduces the unsigned hash modulo the TreeKey maximum uses, constructs a fresh TreeKey, and selects that use index.
4. The selected integer is converted to decimal text and passed to new MiniData(text), whose interpretation is hexadecimal text, not a fixed-width binary integer. Concatenating those bytes with the FORS base seed and hashing derives the per-index FORS seed.
5. Build the FORS tree, sign its root data with TreeKey, and sign the message with FORS. Verification first checks the TreeKey signature on the root data, then FORS on the message; it prints a stage-specific failure and returns false when either check fails.

## Translation work still required

The mapped native counterpart is absent. These notes explain the pinned Java construction and identify dependencies to reuse; they do not claim a native implementation or a standardized SPHINCS implementation. The repository-specific serialization, MMR sum values, and seed derivation must be preserved exactly. TreeKey, Crypto, MiniData, and FORS must be reused with the exact key-index encoding and source parameters. Replacing the construction with a library SPHINCS API would not preserve this wire/signature format.

## Understanding the tests

The source main derives a key from a fixed seed, signs a fixed message, serializes/re-reads SPHINCSSignature, and prints size and verification. Proposed tests should compare seed derivation, private-key reinitialization, selected index and its MiniData bytes, roots and full signature, then separately mutate the TreeKey signature and FORS component.

- [FORS construction](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/FORS/FORS.java)
- [Combined signature encoding](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCSSignature.java)

This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

No native parity test for this absent counterpart is claimed. Printed Java demonstrations are not assertion-based tests, and agreement on a round trip would not by itself establish signature security. The proposed tests remain future work. Building the forest is expensive; test design should reuse deterministic fixtures where possible without changing the algorithm.

## Check your understanding

Why must decimal index 10 become the bytes obtained by parsing text "10" as hex, rather than an assumed binary encoding of decimal ten?

Pinned Java SHA-256: `3ac529ef2dd400856175514ea73a9a956a8b44a7e2ed1e1ec00d06d159ca6690`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
