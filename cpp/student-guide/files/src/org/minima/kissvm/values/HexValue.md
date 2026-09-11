# HexValue — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/values/HexValue.java](../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java) | [src/org/minima/kissvm/values/HexValue.cpp](../../../../../../../src/org/minima/kissvm/values/HexValue.cpp) |

## What this file does

Wrap bytes as a typed KISS HEX value, including a distinct constructor from numbers.

## Follow the logic

1. Text and MiniData constructors delegate to the byte form. The byte form constructs MiniData and rejects more than MAX_DATA_SIZE bytes.
2. The numeric constructor first rejects negative values, then fractions. It converts the integer using BigInteger.toByteArray, whose signed representation can contain a leading zero byte.
3. Expose bytes/MiniData, print the hexadecimal representation and compare through MiniData.

## How the translation is approached

C++ owns MiniData and returns const byte references. Numeric construction reuses BigInt.toJavaBytes, preserving the leading-sign-byte behavior instead of treating it as the HEX built-in conversion. Numeric negative and fractional inputs now raise the existing JavaNumberFormatException with the source messages. This corrects the category exposed by the newly mapped HexValueTests; complete exception and alias parity remains open.

## What the tests tell us

The driver separately exercises hexnumber for negative, fractional and large values, plus byte sizes 0, 1, 65535, 65536 and 65537. It compares raw bytes as well as type and display text.

The actual [KissOracle.java](../../../../../../../support/KissOracle.java) and [kiss.cpp](../../../../../../../support/kiss.cpp) execute the operations built by [kiss_differential.py](../../../../../../../tools/kiss_differential.py). Their output contains value type, text and underlying HEX/string bytes or number wire bytes. Generic errors are normalized to ERROR; parser errors retain their message. The existing [normal report](../../../../../../../validation/student-linked-evidence/build/kiss-differential-results.json) records 1,451 cases passed and zero failures; no new run was performed for this lesson.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [HexValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java#L17), [HexValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java#L24), [HexValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java#L31), [HexValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java#L49), [getMiniData](../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java#L66), [getRawData](../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java#L74), [getValueType](../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java#L79), [isEqual](../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java#L83), [toString](../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java#L88).

## Check your understanding

Trace why constructing HexValue from the number 128 can need a leading zero byte. Which constructor enforces the byte-size limit?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `0847c2e2450cc220fc9ea7368319fbe26afb018fc08efa642d4eb84ef3410029`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).

## New mapped-test evidence

[HexValueTests](../../../../../test/org/minima/tests/kissvm/values/HexValueTests.md) explains all 35 active assertions, including exact invalid-number categories, six equality routes and the source’s repeated inequality direction. The original Java test is executed unchanged beside the native test; full normal and sanitizer builds pass. All 35 assertions pass in each runtime, and all eight affected suites pass in both builds. The broader value/VM/object reports have zero failures.
