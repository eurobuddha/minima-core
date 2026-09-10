# HORSTSignature — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/HORSTSignature.java) · `src/org/minima/utils/sphincs/HORST/HORSTSignature.cpp` (absent)

## Purpose

HORSTSignature stores parallel lists of revealed secret values and public-tree proofs. Serialization pairs each value with the proof at the same index; the class itself does not enforce that the lists have equal length.

## Follow the algorithm

1. Construction initializes two empty ArrayLists. Getters return the live lists so signing code can append directly.
2. writeDataStream writes the value-list length as MiniNumber, then alternates MiniData value and MMRProof for each index. A shorter proof list can fail after a prefix is already written; extra proofs are ignored.
3. readDataStream replaces both lists, reads a count, then appends each value followed by its proof. Failure between these two reads can leave unequal list lengths. A negative narrowed count skips the loop.
4. JSON output emits values containing sigvalue hex and serialized sigproof hex. convertMiniDataVersion catches IOException, logs it, and returns the same possibly partially populated object; other exceptions can propagate.

## Translation work still required

The mapped native counterpart is absent. These notes explain the pinned Java construction and identify dependencies to reuse; they do not claim a native implementation or a standardized SPHINCS implementation. The repository-specific serialization, MMR sum values, and seed derivation must be preserved exactly. Reuse MiniNumber count encoding, MiniData, MMRProof, and the logging/error boundary. A temporary all-or-nothing decode would change the observable partial state.

## Understanding the tests

The HORST main writes a full signature, re-reads it through convertMiniDataVersion, and prints verification. Proposed tests should compare exact alternating wire bytes, every truncated prefix, mismatched input lists, negative counts, trailing bytes, and whether the conversion helper returns partial state while direct reading throws.

- [Signature producer, verifier, and printed round trip](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/HORST.java)

This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

No native parity test for this absent counterpart is claimed. Printed Java demonstrations are not assertion-based tests, and agreement on a round trip would not by itself establish signature security. The proposed tests remain future work.

## Check your understanding

If the first secret value reads successfully but its proof is truncated, what are the two list lengths after the helper catches IOException?

Pinned Java SHA-256: `cddb5f6240bad354beba9d7129f93307c530c496ea707bdaebd4fe8d5c847fe8`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
