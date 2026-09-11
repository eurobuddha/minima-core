# PublicKey — teaching guide

**Teaching status:** Authored explanation of the pinned source, native translation, and concrete validation. Full parity remains unverified.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/PublicKey.java) · [Native counterpart](../../../../../../../../src/org/minima/utils/sphincs/HORST/PublicKey.cpp)

## Purpose

PublicKey hashes each secret preimage and commits all public hashes to an MMR. The leaf’s numeric sum field carries its index, binding a proof to a position as well as a hash.

## Follow the algorithm

1. Allocate one public value per private-key entry and create an empty MMR.
2. For index i, hash the raw private value bytes with Crypto.hashData to form the public value.
3. Create an MMR leaf from that public value and MiniNumber(i), then append it. This leaf construction is distinct from simply inserting the raw public hash.
4. After all entries, finalize the MMR and cache its root. getKey/getSize expose the stored array, getPublicKeyTreeRoot returns the cached MMRData, and getKeyTreeProof requests a proof at the selected MMREntryNumber.

## How the C++ translation works

The native class reuses the existing MMR without changing its hashing, indexed sums, or finalization. Public values and the cached root use shared ownership. A complete 65,536-leaf tree has one peak. Java returns an empty proof for an out-of-range leaf in this tree: both proof loops are empty even though the leaf data is null. The general native MMR rejects missing leaves, so this caller uses its existing getProofToPeak for out-of-range positions. In-range proofs still use the unchanged complete getProof path.

## Understanding the tests

For each of three real full-size trees, tests compare all-public-value digests and the complete root against both Java and an independent Python tree. The expected sum is 2,147,450,880. Selected first/last and out-of-range proofs are compared byte for byte. Python independently reconstructs each generated signature proof from its public value and index. Changed root sums are explicitly rejected, even when the root hash is unchanged.

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

Why does verification reconstruct a leaf with both the public hash and its index instead of proving only membership of the hash?

Pinned Java SHA-256: `48d92b2fd385c83d0b6bbdbbd5cafea88451965f6b4ba2422962cd4015ef5296`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
