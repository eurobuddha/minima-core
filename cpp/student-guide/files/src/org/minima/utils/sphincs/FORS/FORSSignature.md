# FORSSignature — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/FORS/FORSSignature.java) · `src/org/minima/utils/sphincs/FORS/FORSSignature.cpp` (absent)

## Purpose

FORSSignature combines one HORSTSignature with lists of HORST roots and their outer forest proofs. The encoded count for the outer lists is separate from the embedded signature’s count.

## Follow the algorithm

1. Construction creates an empty HORSTSignature and empty root/proof lists; getters expose them directly.
2. writeDataStream writes the embedded signature first, then writes its value-list size again as a MiniNumber count. It follows with MMRData root and MMRProof pairs at each index.
3. readDataStream replaces the embedded signature and reads it, then resets the outer lists and reads their independent count. It does not assert that this count equals the embedded signature size.
4. JSON output includes embedded horstsignature and an array of root/proof descriptions indexed by the embedded value count. An apparently unused root serialization call still executes before its JSON fields are emitted and can fail.
5. convertMiniDataVersion logs IOException and returns partial state. A mismatch between stored counts may only become visible later during JSON output or verification.

## Translation work still required

The mapped native counterpart is absent. These notes explain the pinned Java construction and identify dependencies to reuse; they do not claim a native implementation or a standardized SPHINCS implementation. The repository-specific serialization, MMR sum values, and seed derivation must be preserved exactly. Reuse the exact HORSTSignature, MMRData, MMRProof, and MiniNumber encodings; merging the two wire counts or validating them early would change source behavior.

## Understanding the tests

The FORS demonstration exercises a normal complete serialization round trip and prints verification. Proposed tests should deliberately encode different embedded and outer counts, truncate between a root and its proof, add unused extra roots, and compare direct-reader exceptions versus helper-returned partial objects.

- [Signature producer and printed round trip](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/FORS/FORS.java)

This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

No native parity test for this absent counterpart is claimed. Printed Java demonstrations are not assertion-based tests, and agreement on a round trip would not by itself establish signature security. The proposed tests remain future work.

## Check your understanding

Why are two length fields present, and why does a decoder that silently forces them equal fail to reproduce this Java source?

Pinned Java SHA-256: `5497bc3135f0a17609a5eb7fa74277e7f424762156003636838f4730aa19792f`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
