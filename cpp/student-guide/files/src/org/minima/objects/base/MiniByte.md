# MiniByte — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/base/MiniByte.java](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java) | [src/org/minima/objects/base/MiniByte.cpp](../../../../../../../src/org/minima/objects/base/MiniByte.cpp) |

## What this file does

A single stored byte with two different numeric views: unsigned for protocol fields, signed when matching Java byte values.

## Follow the logic

1. The integer constructor casts to Java byte, retaining only the low eight bits. The boolean constructor produces exactly 0 or 1.
2. getValue masks with 0xFF and therefore returns 0 through 255. getByteValue returns the signed byte, -128 through 127. The same stored bits can consequently print as 255 or -1 depending on the getter.
3. Equality calls both unsigned getters. With the ordinary unmodified FALSE object, isFalse means zero; mutable FALSE and subclass overrides can change that result.
4. Serialization writes exactly one byte. Reading consumes one byte with no separate length prefix; toString uses the unsigned value.

## How the translation is approached

Native storage is uint8_t. The int constructor performs the same narrowing, and bit_cast<int8_t> provides the signed view without relying on implementation-defined signed conversion. This reuses the established primitive counterpart documented in its source header.

## What the tests tell us

The driver tests every integer from -512 through 512, comparing unsigned text, signed getter and wire byte. It therefore traverses the byte range repeatedly and exercises wraparound. It does not directly call isTrue/isFalse, so those semantics here come from reading the bodies.

The [primitive differential driver](../../../../../../../tools/differential.py) compiles the pinned Java classes and runs [PrimitiveOracle](../../../../../../../support/PrimitiveOracle.java) against [minima_primitives](../../../../../../../support/primitives.cpp). Both report observable text and wire bytes; both flatten exceptions to `ERROR`. The inspected [existing report](../../../../../../../validation/checkpoint-362/normal/differential-results.json) records 3,802/3,802 comparisons with zero failures for the complete primitive corpus. This does not prove exact error classes, all methods, or full-node parity.

## Dependencies and reading map

Direct Minima imports: [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [MiniByte](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L19), [MiniByte](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L23), [MiniByte](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L27), [MiniByte](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L31), [getValue](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L39), [getByteValue](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L43), [isEqual](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L47), [isFalse](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L51), [isTrue](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L55), [toString](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L60), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L65), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L70), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L74), [WriteToStream](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java#L80).

## Check your understanding

Explain why MiniByte(257) has unsigned value 1 and why MiniByte(255).isTrue is true even though its signed value is -1.

## Common interface integration

This class now implements the native Streamable interface. Its existing byte
format is retained; reads dispatch into the same object and writers append to the
supplied output. The [interface lab](../../../../../../new-tests/streamable-interface.md) tests base-reference calls,
partial reads and existing generic file/hash/network-message helpers. Both normal and sanitizer
runs pass 909 comparisons and 3,850 assertions across all five adopted types;
this does not establish full interface adoption or complete class parity.

## Limits and traceability

Boolean truth is not restricted to the canonical serialized values 0 and 1. Callers that need canonical validation must perform it separately. The source-map status remains partial and parity not_verified.

Pinned Java SHA-256: `5b94c4e0c9678079998358e10a18fa1ebfafdce0e91acedc3c4e3b9f1920d5b2`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).

## Source-named stream helper audit

The [public-helper lab](../../../../../../new-tests/base-stream-helpers.md) explains
the newly exposed source API methods and their Java/native tests. The existing
base-value corpus is retained and extended to 1,853 cases, including prefix bytes,
truncated reads, nullable polymorphic serialization and retained decimal values.
This is additional scoped evidence, not a full file-parity claim.

FALSE and TRUE now retain mutable native objects. isFalse compares against current
FALSE, matching Java even after a stream read changes that object. The fixture
varies FALSE independently of the queried byte and restores it after each case.

## Virtual dispatch correction

The [inheritance lab](../../../../../../new-tests/base-virtual-dispatch.md) explains
why base-reference calls, calls between methods and source copy constructors must
preserve Java overrides. The regression records ordered calls and exceptions,
with different stored values and overridden results. It retains the prior base
API corpus and adds 2,328 subclass cases. Full class parity remains unverified.
