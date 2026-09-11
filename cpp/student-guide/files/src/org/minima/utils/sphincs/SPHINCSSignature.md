# SPHINCSSignature — teaching guide

**Teaching status:** Authored explanation of the pinned source, native translation, and concrete validation. Full parity remains unverified.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCSSignature.java) · [Native counterpart](../../../../../../../src/org/minima/utils/sphincs/SPHINCSSignature.cpp)

## Purpose

SPHINCSSignature is the ordered envelope for three linked proof components: a Minima TreeKey signature, the FORS MMR root, and the FORS signature.

## Follow the algorithm

1. The empty constructor leaves fields unset. The three-argument constructor retains the supplied component references and getters return them.
2. JSON output uses WOTSsignature, FORSroot, and FORSsignature keys in that insertion order, delegating each component’s JSON conversion.
3. writeDataStream writes Signature, then MMRData root, then FORSSignature without an additional envelope count or tag.
4. readDataStream assigns the first two decoded components sequentially, then allocates and reads a FORSSignature. A failure can therefore leave an object partially updated.
5. convertMiniDataVersion creates an empty envelope, catches/logs IOException from reading, and returns it even after failure. It does not demand exhaustion of trailing input and does not turn all exception categories into null.

## How the C++ translation works

The native envelope has shared state containing three nullable shared component
pointers. The constructor retains them and getters return them. It directly reuses
Signature, MMRData and FORSSignature; it adds no new framing. Signature and
SignatureProof now expose incremental writeDataStream(vector&) methods using their
existing codecs and the shared MMRProof writer. This preserves output already
written before a later proof or component fails.

The reader assigns a new WOTS Signature only after its complete factory read
succeeds. It does the same for the root. It then stores a new FORSSignature before
reading into it. This distinction preserves both old-object aliases and partial
new FORS state. The converter catches only I/O exceptions, logs and returns its
partly filled envelope; null input and runtime BigInteger failures still escape.

## Understanding the tests

The driver checks all null-component combinations, constructor identity, component
mutation aliases, oversized roots/proofs, and malformed counts. Every prefix of a
small independent example and samples throughout an actual full-size FORS signature
exercise consumed offsets and the three assignment boundaries. It compares old/new
field identity, retained old sizes, partial output hash/length, JSON and typed errors.
Python independently computes the expected concatenated wire bytes. The actual FORS
bytes come from a completed pinned Java/native full-forest comparison.

See the [envelope lab](../../../../../../../student-guide/new-tests/sphincs-signature-envelope.md)
and [driver](../../../../../../../tools/sphincs_signature_differential.py). The
source demonstration alone has no assertions. These structural tests do not claim
that their synthetic WOTS components authenticate a FORS root; the full SPHINCS
signing class remains to be ported and integrated.

## Common interface integration

This class now implements the native Streamable interface. Its existing byte
format is retained; reads dispatch into the same object and writers append to the
supplied output. The [interface lab](../../../../../../new-tests/streamable-interface.md) tests base-reference calls,
partial reads and existing generic file/hash/network-message helpers. Both normal and sanitizer
runs pass 909 comparisons and 3,850 assertions across all five adopted types;
this does not establish full interface adoption or complete class parity.

## Limits and a useful next test

The current key model's null SignatureProof internals/list entries, raw Java
MiniData byte-array aliases, arbitrary subclasses, concurrent mutation and arbitrary
failing streams remain outside this validation. NPE/EOF prose and runtime-specific
stack frames are normalized. Full signature-algorithm and node parity are unverified.

## Check your understanding

If the FORS component fails halfway through reading, which earlier envelope components are already assigned and which getter can expose partial data?

Pinned Java SHA-256: `4cdcdb16ee37c8c18299297a2bcb7e948fd8f048369f35b67419bedee1ed4d49`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
