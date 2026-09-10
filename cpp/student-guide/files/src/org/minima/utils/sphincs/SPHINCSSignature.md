# SPHINCSSignature — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCSSignature.java) · `src/org/minima/utils/sphincs/SPHINCSSignature.cpp` (absent)

## Purpose

SPHINCSSignature is the ordered envelope for three linked proof components: a Minima TreeKey signature, the FORS MMR root, and the FORS signature.

## Follow the algorithm

1. The empty constructor leaves fields unset. The three-argument constructor retains the supplied component references and getters return them.
2. JSON output uses WOTSsignature, FORSroot, and FORSsignature keys in that insertion order, delegating each component’s JSON conversion.
3. writeDataStream writes Signature, then MMRData root, then FORSSignature without an additional envelope count or tag.
4. readDataStream assigns the first two decoded components sequentially, then allocates and reads a FORSSignature. A failure can therefore leave an object partially updated.
5. convertMiniDataVersion creates an empty envelope, catches/logs IOException from reading, and returns it even after failure. It does not demand exhaustion of trailing input and does not turn all exception categories into null.

## Translation work still required

The mapped native counterpart is absent. These notes explain the pinned Java construction and identify dependencies to reuse; they do not claim a native implementation or a standardized SPHINCS implementation. The repository-specific serialization, MMR sum values, and seed derivation must be preserved exactly. The three existing component encodings define the format; a new tagged envelope would be incompatible. A native optional/shared-reference representation would need to retain the source’s initially null and partially read states.

## Understanding the tests

The SPHINCS main serializes a complete signature and verifies its reconstructed version, printing the result without an assertion. Proposed tests should compare exact component concatenation, every truncated prefix, trailing bytes, null fields, and reuse of an already initialized object during a failing read.

- [Producer, verifier, and printed round trip](../../../../../../../../upstream-study/src/org/minima/utils/sphincs/SPHINCS.java)

This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

No native parity test for this absent counterpart is claimed. Printed Java demonstrations are not assertion-based tests, and agreement on a round trip would not by itself establish signature security. The proposed tests remain future work.

## Check your understanding

If the FORS component fails halfway through reading, which earlier envelope components are already assigned and which getter can expose partial data?

Pinned Java SHA-256: `4cdcdb16ee37c8c18299297a2bcb7e948fd8f048369f35b67419bedee1ed4d49`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
