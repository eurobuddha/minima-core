# HORSTSignature — teaching guide

**Teaching status:** Authored explanation of the pinned source, native translation, and concrete validation. Full parity remains unverified.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/HORST/HORSTSignature.java) · [Native counterpart](../../../../../../../../src/org/minima/utils/sphincs/HORST/HORSTSignature.cpp)

## Purpose

HORSTSignature stores parallel lists of revealed secret values and public-tree proofs. Serialization pairs each value with the proof at the same index; the class itself does not enforce that the lists have equal length.

## Follow the algorithm

1. Construction initializes two empty ArrayLists. Getters return the live lists so signing code can append directly.
2. writeDataStream writes the value-list length as MiniNumber, then alternates MiniData value and MMRProof for each index. A shorter proof list can fail after a prefix is already written; extra proofs are ignored.
3. readDataStream replaces both lists, reads a count, then appends each value followed by its proof. Failure between these two reads can leave unequal list lengths. A negative narrowed count skips the loop.
4. JSON output emits values containing sigvalue hex and serialized sigproof hex. convertMiniDataVersion catches IOException, logs it, and returns the same possibly partially populated object; other exceptions can propagate.

## How the C++ translation works

The native signature uses shared state containing two separately shared vectors, following the existing MegaMMRSyncData ownership pattern. Reading replaces both list objects before the count, leaving previously returned list handles detached and intact. Values and proofs are appended separately in source order. writeDataStream appends to a supplied byte vector so a failed write retains its prefix. Proof fields now use the extracted MMRProof::writeDataStream loop, shared with FORSSignature. JSON proof conversion catches an I/O error, logs it, then throws a null-pointer error just as Java does when it dereferences MiniData.getMiniDataVersion's null return. The conversion helper instead returns the partially populated signature after I/O errors.

## Understanding the tests

The driver truncates every prefix of an independently encoded small signature and samples every part of a full signature. Each case compares consumed bytes, new list lengths, old-list identity and lengths, partial output bytes through their digest, JSON or its failure, and typed read/write exceptions. Converter cases distinguish caught I/O errors from an escaping zero-length BigInteger error. Mutations exercise each of the sixteen slots, unequal lists, null entries, and oversized proof hashes that fail after part of the output has been written.

The [HORST lab](../../../../../../../../student-guide/new-tests/horst-signatures.md)
explains the independent reconstruction and records completed execution results.
The [test driver](../../../../../../../../tools/horst_differential.py) calls the
unmodified Java sources and the compiled native library. The source demonstration
alone contains no assertions; the driver adds explicit acceptance and rejection checks.

## Common interface integration

This class now implements the native Streamable interface. Its existing byte
format is retained; reads dispatch into the same object and writers append to the
supplied output. The [interface lab](../../../../../../../new-tests/streamable-interface.md) tests base-reference calls,
partial reads and existing generic file/hash/network-message helpers. Both normal and sanitizer
runs pass 909 comparisons and 3,850 assertions across all five adopted types;
this does not establish full interface adoption or complete class parity.

## Limits and a useful next test

These cases cover the specified algorithm and error boundaries, not all possible
Java subclass overrides, concurrent mutation, or mutable raw Java byte-array aliases.
Null-pointer/EOF prose and runtime-specific stack frames are normalized by the fixtures.
The Java test VM disables fast-throw diagnostic omission so repeated array errors
retain comparable messages. FORS/SPHINCS integration and full node parity remain open.
Passing these tests is not a cryptographic security certification.

## Check your understanding

If the first secret value reads successfully but its proof is truncated, what are the two list lengths after the helper catches IOException?

Pinned Java SHA-256: `cddb5f6240bad354beba9d7129f93307c530c496ea707bdaebd4fe8d5c847fe8`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).
