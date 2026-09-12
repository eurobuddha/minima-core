# MiniString — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/base/MiniString.java](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java) | [src/org/minima/objects/base/MiniString.cpp](../../../../../../../src/org/minima/objects/base/MiniString.cpp) |

## What this file does

The UTF-8 text wrapper used by Minima serialization. It deliberately decodes and re-encodes data, so malformed byte sequences are normalized.

## Follow the logic

1. The Java String constructor first encodes UTF-8, then delegates to the byte constructor, which decodes UTF-8. Invalid surrogate/text input can therefore be replaced during construction.
2. The byte constructor decodes with Java UTF-8 replacement behavior. getData encodes the normalized stored string again; it does not retain the original malformed bytes.
3. isEqual compares text. The copy constructor copies the other wrapper’s string.
4. Serialization wraps getData in MiniData, giving a four-byte big-endian byte count followed by UTF-8 bytes. Reading decodes a MiniData payload back to text.
5. The count is bytes rather than Java UTF-16 code units or visible characters. A panda emoji occupies four UTF-8 bytes although it is one Unicode code point.

## How the translation is approached

C++ stores UTF-8 in std::string and normalizes with the shared UTF8Spans decoder. Each invalid span becomes U+FFFD encoded as EF BF BD. Native length and subset helpers operate on byte positions and are additional APIs, not direct counterparts of Java String.length or substring.

## What the tests tell us

The fixture includes empty text, ASCII, euro and panda characters; every individual byte; 2,000 seeded random byte sequences; and explicit malformed surrogate, overlong, out-of-range and truncated UTF-8 sequences. It compares the reserialized normalized bytes. These are byte-input tests, not exhaustive tests of every possible Java String constructor input.

The [primitive differential driver](../../../../../../../tools/differential.py) compiles the pinned Java classes and runs [PrimitiveOracle](../../../../../../../support/PrimitiveOracle.java) against [minima_primitives](../../../../../../../support/primitives.cpp). Both report observable text and wire bytes; both flatten exceptions to `ERROR`. The inspected [existing report](../../../../../../../validation/checkpoint-362/normal/differential-results.json) records 3,802/3,802 comparisons with zero failures for the complete primitive corpus. This does not prove exact error classes, all methods, or full-node parity.

## Dependencies and reading map

Direct Minima imports: [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [MiniString](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java#L22), [MiniString](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java#L26), [MiniString](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java#L30), [isEqual](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java#L34), [toString](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java#L39), [getData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java#L43), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java#L48), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java#L54), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java#L59), [WriteToStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java#L65).

## Check your understanding

Predict the length prefix for the panda emoji. Why can serializing malformed input produce more bytes than were originally supplied?

## Limits and traceability

Java exposes MINIMA_CHARSET as a mutable static Charset; the native wrapper is fixed to UTF-8. Charset mutation and byte-subset extension behavior are outside the reported primitive fixture. The source-map status remains partial and parity not_verified.

Pinned Java SHA-256: `af116863598a633df10863b96e3c24c1d717e7d698ed347a3b4a2326c7228eac`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).

## Scalar Streamable integration

The native class now implements Streamable. Its writer delegates to MiniData,
matching the source's framing directly. The reader waits for a complete MiniData
payload and UTF-8 normalization before replacing the stored string. An incomplete
payload leaves the previous string intact, even though the nested temporary data
reader would have partial bytes. The suite includes malformed surrogate, overlong
and truncated UTF-8 encodings. Mutable MINIMA_CHARSET and complete null/reference
APIs remain unfinished.

See the [scalar interface lab](../../../../../../new-tests/streamable-scalars.md) for the test logic. Both expanded interface suites pass 2,095 comparisons and 12,220 assertions across 14 types with matching transcripts and empty native stderr. All nine affected regressions pass in both builds. Full class parity remains unverified.

## Source-named stream helper audit

The [public-helper lab](../../../../../../new-tests/base-stream-helpers.md) explains
the newly exposed source API methods and their Java/native tests. The existing
base-value corpus is retained and extended to 1,853 cases, including prefix bytes,
truncated reads, nullable polymorphic serialization and retained decimal values.
This is additional scoped evidence, not a full file-parity claim.
