# FORS — teaching guide

**Teaching status:** Authored explanation of the pinned source, native translation, and concrete validation. Full parity remains unverified.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/FORS/FORS.java) · [Native counterpart](../../../../../../../../src/org/minima/utils/sphincs/FORS/FORS.cpp)

## Purpose

FORS combines 16 separate HORST trees into a forest commitment. One selected preimage is revealed from each tree, with both an inner public-key proof and an outer forest proof.

## Follow the algorithm

1. For each tree index i from 0 to 15, derive a seed by hashing serialized base seed and MiniNumber(i), build that HORST tree, and add an outer MMR leaf containing its root data with sum i. Finalize the forest MMR.
2. Signing hashes raw message bytes once. Chunk i selects a two-byte key index in HORST tree i. Append its private preimage and inner public-key proof to the embedded HORSTSignature.
3. Also append that HORST root and a forest proof at outer index i. The four parallel components identify both which tree and which key within it are used.
4. Verification first proves each supplied HORST root at outer index i against the forest root. It then hashes the supplied secret, reconstructs the inner leaf using the message-derived index, and verifies its proof against that HORST root.
5. Return false at the first root mismatch, true after all 16 pairs. Missing/null lists can throw because structural lengths are assumed rather than prevalidated.

## How the C++ translation works

The C++ constructor retains sixteen complete HORST objects and uses the existing MMR for the outer tree. Seed derivation, leaf sums, finalization and both proof calculations reuse the tested primitives. Shared roots and selected secrets retain their identity across signatures; each signature gets separate lists and proof objects. Verification follows Java's outer-before-inner order without adding an early structural validation pass. The source log and main are translated as well.

## Understanding the tests

The driver constructs all 1,048,576 leaves in each runtime and independently rebuilds sixteen inner roots and the outer root with Python SHA3. It compares exact signatures for four messages, including an actual hash with a repeated index, and supplies both writers' signatures to both verifiers. Every inner and outer proof is reconstructed independently. Mutations at all sixteen positions distinguish wrong outer roots, wrong inner sums, wrong secrets, empty proofs, short lists and null entries. Alias cases check shared roots/secrets and distinct lists/proofs. The source main is invoked directly.

The [FORS lab](../../../../../../../../student-guide/new-tests/fors-signatures.md)
records the plan and execution results. The [driver](../../../../../../../../tools/fors_differential.py)
calls unmodified pinned Java classes and the compiled native library.

## Limits and a useful next test

Raw MiniData byte-array aliases, arbitrary Java subclasses, concurrent mutation,
arbitrary failing output streams, and full SPHINCS/node integration remain open.
The fixtures normalize NPE/EOF prose and runtime-specific stack frames; logged
exception counts/categories are compared. Passing these tests does not establish
cryptographic security or complete file parity.

## Check your understanding

Why does proving a preimage under a supplied HORST root not suffice until that HORST root is also proven under the forest root?

Pinned Java SHA-256: `5793254152bf14fda00f675689feb43f13d38b755d300200cd8cda01dc10094b`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
