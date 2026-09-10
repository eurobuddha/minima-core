# HORST — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/HORST.java) · `src/org/minima/utils/sphincs/HORST/HORST.cpp` (absent)

## Purpose

HORST reveals selected secret preimages and accompanies each with an MMR proof under one public-key root. A 32-byte message hash selects 16 indices, each represented by two adjacent bytes.

## Follow the algorithm

1. Construction derives a PrivateKey and then builds PublicKey from it. The HORST root is the public key’s MMR root.
2. signMessage hashes the raw message bytes. For positions 0 through 15, getKeyRef copies two bytes at offsets 2*i and 2*i+1 and interprets them through MiniData as an unsigned integer.
3. For each index, append that secret preimage and the matching public-tree proof to parallel signature lists. Repeated two-byte chunks may select the same index more than once.
4. Verification hashes the message again, recomputes each index, hashes the supplied preimage, reconstructs an MMR leaf with that index, and calculates the supplied proof. Any root mismatch returns false; all 16 matches return true.
5. The method assumes the required list entries exist. It does not first validate list lengths, so a truncated signature may throw rather than simply return false. Extra entries after the 16 used values are ignored by the verification loop.

## Translation work still required

The mapped native counterpart is absent. These notes explain the pinned Java construction and identify dependencies to reuse; they do not claim a native implementation or a standardized SPHINCS implementation. The repository-specific serialization, MMR sum values, and seed derivation must be preserved exactly. A native translation should reuse the existing Crypto/MiniData/MMR proof primitives, preserving the distinction between raw-message hashing and structured seed expansion.

## Understanding the tests

The source main uses fixed seed/message values, serializes and re-reads a signature, and prints Verify plus size/root. It contains no assertions. Proposed differential cases should compare index extraction at 0x0000/0x00FF/0xFF00/0xFFFF, all signature bytes, successful verification, changed message/preimage/proof/root, duplicate selected indices, and too-short/extra lists.

- [Deterministic secret expansion](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/PrivateKey.java)
- [Indexed public MMR construction](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/PublicKey.java)

This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

No native parity test for this absent counterpart is claimed. Printed Java demonstrations are not assertion-based tests, and agreement on a round trip would not by itself establish signature security. The proposed tests remain future work.

## Check your understanding

If two chunks of the message hash are identical, how many signature list entries are still appended and which secret values do they contain?

Pinned Java SHA-256: `320f0ac46e9e2a0ab9556ee0fa3f69bbe8006890a8b8b6d12d8465d3a0eda904`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
