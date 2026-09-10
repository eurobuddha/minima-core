# StringValue — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/values/StringValue.java](../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java) | [src/org/minima/kissvm/values/StringValue.cpp](../../../../../../../src/org/minima/kissvm/values/StringValue.cpp) |

## What this file does

Store KISS text through MiniString, with an encoded-byte size limit.

## Follow the logic

1. Construct MiniString from the supplied Java string. This performs the source encoding round trip.
2. Measure getBytes().length and reject a value larger than MAX_DATA_SIZE. This limit counts bytes, not visible characters.
3. Expose text, bytes and MiniString; compare text equality.
4. Concatenation constructs a new StringValue, so conversion and size checks run again.

## How the translation is approached

C++ stores both MiniString and decoded UTF-16. Constructing from UTF-16 first encodes through MiniString and decodes again, replacing isolated surrogates with question marks just as the pinned path does.

## What the tests tell us

The value driver tests ordinary, empty and emoji strings and sizes 0, 1, 16383, 16384 and 16385 repeated pandas. Four UTF-8 bytes per panda bring the final two cases to the byte-limit boundary. The execution suite also splits surrogate pairs through SUBSTR and observes reconstruction.

The actual [KissOracle.java](../../../../../../../support/KissOracle.java) and [kiss.cpp](../../../../../../../support/kiss.cpp) execute the operations built by [kiss_differential.py](../../../../../../../tools/kiss_differential.py). Their output contains value type, text and underlying HEX/string bytes or number wire bytes. Generic errors are normalized to ERROR; parser errors retain their message. The existing [normal report](../../../../../../../build/kiss-differential-results.json) records 1,451 cases passed and zero failures; no new run was performed for this lesson.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [MiniString](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java).

Method navigation (declarations with bodies found in this file): [StringValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java#L13), [toString](../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java#L23), [getBytes](../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java#L27), [getMiniString](../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java#L31), [getValueType](../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java#L36), [isEqual](../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java#L40), [add](../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java#L44).

## Check your understanding

Why do 16,384 pandas fit a 65,536-byte limit while 16,385 fail?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `5a6886c449be62548f3bfcb4f9f2267c2d50ba51aed1c015e7da46c28f2eab19`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
