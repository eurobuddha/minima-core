# PublicKey — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/PublicKey.java) · `src/org/minima/utils/sphincs/HORST/PublicKey.cpp` (absent)

## Purpose

PublicKey hashes each secret preimage and commits all public hashes to an MMR. The leaf’s numeric sum field carries its index, binding a proof to a position as well as a hash.

## Follow the algorithm

1. Allocate one public value per private-key entry and create an empty MMR.
2. For index i, hash the raw private value bytes with Crypto.hashData to form the public value.
3. Create an MMR leaf from that public value and MiniNumber(i), then append it. This leaf construction is distinct from simply inserting the raw public hash.
4. After all entries, finalize the MMR and cache its root. getKey/getSize expose the stored array, getPublicKeyTreeRoot returns the cached MMRData, and getKeyTreeProof requests a proof at the selected MMREntryNumber.

## Translation work still required

The mapped native counterpart is absent. These notes explain the pinned Java construction and identify dependencies to reuse; they do not claim a native implementation or a standardized SPHINCS implementation. The repository-specific serialization, MMR sum values, and seed derivation must be preserved exactly. MiniData hashing, MMR leaf creation, finalization, and proof generation are the lower-level dependencies. The full MMRData root includes more than just an unlabelled hash.

## Understanding the tests

The HORST demonstration prints the public root and uses generated proofs in a signature. Proposed tests should compare the complete fixed-seed root and proofs for first/last indices, recalculate each proof from its correct indexed leaf, and show that substituting a different index changes verification even when the public hash is unchanged.

- [Java signature and demonstration caller](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/HORST.java)

This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

No native parity test for this absent counterpart is claimed. Printed Java demonstrations are not assertion-based tests, and agreement on a round trip would not by itself establish signature security. The proposed tests remain future work.

## Check your understanding

Why does verification reconstruct a leaf with both the public hash and its index instead of proving only membership of the hash?

Pinned Java SHA-256: `48d92b2fd385c83d0b6bbdbbd5cafea88451965f6b4ba2422962cd4015ef5296`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
