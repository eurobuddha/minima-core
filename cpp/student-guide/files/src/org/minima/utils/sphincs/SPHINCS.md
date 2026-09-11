# SPHINCS — teaching guide

**Teaching status:** Authored explanation of the Java source, native composition, and completed full-size validation and remaining limits.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCS.java) · [Native C++ implementation](../../../../../../../src/org/minima/utils/sphincs/SPHINCS.cpp)

## Purpose

This repository-specific construction joins a Minima TreeKey signature to a FORS signature. The TreeKey authenticates the FORS root, and FORS authenticates the message. The class name alone does not establish equivalence to a standardized SPHINCS scheme.

## Follow the algorithm

1. initSeed hashes the original seed concatenated with WOTS and separately FORS text to derive two 32-byte seeds. It concatenates those seeds as the private key, builds a TreeKey with 192 keys per level and depth 4, and records its public root and maximum uses.
2. initPrivateKey retains the supplied private-key value, copies its first 32 and next 32 bytes into the two seeds, and derives the same TreeKey public root. Too-short data fails while extra bytes are retained in the stored private key but ignored by seed extraction.
3. signMessage hashes raw message bytes, reduces the unsigned hash modulo the TreeKey maximum uses, constructs a fresh TreeKey, and selects that use index.
4. The selected integer is converted to decimal text and passed to new MiniData(text), whose interpretation is hexadecimal text, not a fixed-width binary integer. Concatenating those bytes with the FORS base seed and hashing derives the per-index FORS seed.
5. Build the FORS tree, sign its root data with TreeKey, and sign the message with FORS. Verification first checks the TreeKey signature on the root data, then FORS on the message; it prints a stage-specific failure and returns false when either check fails.

## Native translation and reuse

The native class directly calls TreeKey with 192 keys per level and four levels,
then the existing full-size FORS implementation. It retains the private-key input
reference before checking its bytes. The interleaved copy loop reproduces the
first failing index for every short length; the old seeds/public key survive.
The nullable total count and public key preserve the default uninitialized state.
Verification checks WOTS before FORS and preserves each stage's printed failure.

Two shared key error paths were aligned with Java: null public-key comparison
throws at the first proof comparison (empty or overlong proof lists can return
false first), and an invalid WOTS signature length throws NullPointerException.
Neither change replaces the existing key generation or hashing algorithm.

## Understanding the tests

The [signer lab](../../../../../../../student-guide/new-tests/sphincs-signer.md)
was written before execution. The new driver invokes the unmodified pinned Java
class and the actual compiled native class. It checks every short private-key
length before/after initialization, input identity, retained old fields, seed
suffix hashing, extra private bytes, and recovery after failed reinitialization.
Full signatures compare all bytes and JSON, then both readers verify both writers.
Python independently checks leaf selection, the decimal-text-as-hex encoding,
selected secrets, all FORS paths, selected WOTS keys, chains, and MMR paths.

The normal and sanitized full builds pass. Object/key and envelope regressions also pass in both builds; see [current evidence](../../../../../../../validation/sphincs-work-in-progress/progress.json).
The resumed full-size sanitizer suite and actual source mains now pass; details below.

## Limits and a useful next test

Raw mutable Java MiniData arrays, arbitrary subclasses and concurrent mutation
remain unverified. Existing SignatureProof null internals/list entries are not yet
represented. The public key construction and signature size are those of the
pinned repository; agreement does not establish security or standardized SPHINCS
conformance. Full transaction/script integration remains subsequent work.

## Check your understanding

Why must decimal index 10 become the bytes obtained by parsing text "10" as hex, rather than an assumed binary encoding of decimal ten?

Pinned Java SHA-256: `3ac529ef2dd400856175514ea73a9a956a8b44a7e2ed1e1ec00d06d159ca6690`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).

The original sanitizer run stopped on a response timeout during its third
(empty-message) signature. Its first 158 comparisons and two complete signature
pairs matched; these records and the timeout log are preserved. The resumed
suite uses those records explicitly as saved observations, rechecks their proofs
and actual verifiers, then generates the missing full-size signature and runs the
source mains. Its 3,600-second per-operation timeout changes only test orchestration.
The resumed sanitizer suite finished successfully: 299 live comparisons and 1,377 independent assertions passed in 3,309.726 seconds. It rechecked the two saved signature records, generated the missing full-size signature, and matched both actual source demonstrations. Native process diagnostics were empty. The original timeout remains recorded; these results do not certify the remaining API and full-node gaps.
