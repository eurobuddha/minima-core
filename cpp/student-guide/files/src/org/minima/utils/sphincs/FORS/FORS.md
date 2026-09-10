# FORS — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/FORS/FORS.java) · `src/org/minima/utils/sphincs/FORS/FORS.cpp` (absent)

## Purpose

FORS combines 16 separate HORST trees into a forest commitment. One selected preimage is revealed from each tree, with both an inner public-key proof and an outer forest proof.

## Follow the algorithm

1. For each tree index i from 0 to 15, derive a seed by hashing serialized base seed and MiniNumber(i), build that HORST tree, and add an outer MMR leaf containing its root data with sum i. Finalize the forest MMR.
2. Signing hashes raw message bytes once. Chunk i selects a two-byte key index in HORST tree i. Append its private preimage and inner public-key proof to the embedded HORSTSignature.
3. Also append that HORST root and a forest proof at outer index i. The four parallel components identify both which tree and which key within it are used.
4. Verification first proves each supplied HORST root at outer index i against the forest root. It then hashes the supplied secret, reconstructs the inner leaf using the message-derived index, and verifies its proof against that HORST root.
5. Return false at the first root mismatch, true after all 16 pairs. Missing/null lists can throw because structural lengths are assumed rather than prevalidated.

## Translation work still required

The mapped native counterpart is absent. These notes explain the pinned Java construction and identify dependencies to reuse; they do not claim a native implementation or a standardized SPHINCS implementation. The repository-specific serialization, MMR sum values, and seed derivation must be preserved exactly. The existing MMRData and MMRProof operations are necessary for both levels. A single-tree HORST signature cannot substitute for the outer forest binding.

## Understanding the tests

The source main constructs a fixed-seed forest, signs fixed bytes, serializes/re-reads the signature, and prints verification and size. Proposed tests should compare all outer roots/proofs and wire bytes, then independently alter outer tree index, inner key index, preimage, proof, message, and root to identify the failing verification layer.

- [Inner-tree index and key operations](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/HORST.java)
- [Four-component signature encoding](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/FORS/FORSSignature.java)

This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

No native parity test for this absent counterpart is claimed. Printed Java demonstrations are not assertion-based tests, and agreement on a round trip would not by itself establish signature security. The proposed tests remain future work.

## Check your understanding

Why does proving a preimage under a supplied HORST root not suffice until that HORST root is also proven under the forest root?

Pinned Java SHA-256: `5793254152bf14fda00f675689feb43f13d38b755d300200cd8cda01dc10094b`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
