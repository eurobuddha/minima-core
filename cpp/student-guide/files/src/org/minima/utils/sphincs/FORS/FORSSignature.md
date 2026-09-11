# FORSSignature — teaching guide

**Teaching status:** Authored explanation of the pinned source, native translation, and concrete validation. Full parity remains unverified.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/utils/sphincs/FORS/FORSSignature.java) · [Native counterpart](../../../../../../../../src/org/minima/utils/sphincs/FORS/FORSSignature.cpp)

## Purpose

FORSSignature combines one HORSTSignature with lists of HORST roots and their outer forest proofs. The encoded count for the outer lists is separate from the embedded signature’s count.

## Follow the algorithm

1. Construction creates an empty HORSTSignature and empty root/proof lists; getters expose them directly.
2. writeDataStream writes the embedded signature first, then writes its value-list size again as a MiniNumber count. It follows with MMRData root and MMRProof pairs at each index.
3. readDataStream replaces the embedded signature and reads it, then resets the outer lists and reads their independent count. It does not assert that this count equals the embedded signature size.
4. JSON output includes embedded horstsignature and an array of root/proof descriptions indexed by the embedded value count. An apparently unused root serialization call still executes before its JSON fields are emitted and can fail.
5. convertMiniDataVersion logs IOException and returns partial state. A mismatch between stored counts may only become visible later during JSON output or verification.

## How the C++ translation works

The native class holds shared state with an embedded HORSTSignature and separately shared outer root/proof lists. Reading first replaces the inner object. Only after its read succeeds are the outer lists replaced. Counts are decoded independently; serialization still writes the inner count twice. The existing HORSTSignature writer is reused, and its tested incremental proof-writing loop is now shared through MMRProof::writeDataStream. The unused root serialization call in toJSON is retained: an I/O error logs but its null result is ignored, whereas a failed proof conversion is dereferenced and throws NPE.

## Understanding the tests

The driver truncates each prefix of a small independently encoded nested signature and samples every part of a full signature. It compares consumed byte counts, partial output hashes and lengths, JSON or typed errors, old/new object identity and list lengths. Unequal and narrowed counts, null entries, missing lists, oversized roots and oversized proof hashes exercise distinct failure orders. Converter cases verify that I/O errors log and return partial objects while zero-length BigInteger runtime errors escape. A source demonstration alone has no assertions; these tests add explicit checks.

The [FORS lab](../../../../../../../../student-guide/new-tests/fors-signatures.md)
records the plan and execution results. The [driver](../../../../../../../../tools/fors_differential.py)
calls unmodified pinned Java classes and the compiled native library.

## Common interface integration

This class now implements the native Streamable interface. Its existing byte
format is retained; reads dispatch into the same object and writers append to the
supplied output. The [interface lab](../../../../../../../new-tests/streamable-interface.md) tests base-reference calls,
partial reads and existing generic file/hash/network-message helpers. Both normal and sanitizer
runs pass 909 comparisons and 3,850 assertions across all five adopted types;
this does not establish full interface adoption or complete class parity.

## Limits and a useful next test

Raw MiniData byte-array aliases, arbitrary Java subclasses, concurrent mutation,
arbitrary failing output streams, and full SPHINCS/node integration remain open.
The fixtures normalize NPE/EOF prose and runtime-specific stack frames; logged
exception counts/categories are compared. Passing these tests does not establish
cryptographic security or complete file parity.

## Check your understanding

Why are two length fields present, and why does a decoder that silently forces them equal fail to reproduce this Java source?

Pinned Java SHA-256: `5497bc3135f0a17609a5eb7fa74277e7f424762156003636838f4730aa19792f`. [Source mapping](../../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../../REVIEW.md).

## Shared conversion helper

The SPHINCS transaction integration extracts the existing serialization/I/O-log helper into [MiniDataVersion.hpp](../../../../../../../../src/support/MiniDataVersion.hpp). The helper body and catch boundary are unchanged; envelope and partial-transaction tests exercise it.
