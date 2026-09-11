# NumberValue — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/values/NumberValue.java](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java) | [src/org/minima/kissvm/values/NumberValue.cpp](../../../../../../../src/org/minima/kissvm/values/NumberValue.cpp) |

## What this file does

Expose Minima decimal arithmetic to the KISS evaluator.

## Follow the logic

1. Construct MiniNumber from an integer/string or retain the supplied MiniNumber.
2. Delegate comparisons and the zero-is-false helper to MiniNumber.
3. For addition, subtraction, multiplication and division, return a new NumberValue containing the corresponding MiniNumber result.
4. Return the NUMBER type tag and MiniNumber text.

## How the translation is approached

C++ stores MiniNumber by value and reuses its decimal operations. It does not convert values through double. Java reference aliasing to a supplied MiniNumber and the native value model are not fully audited by these arithmetic cases.

## What the tests tell us

The number cases cross 0, 1, -1, 123.456 and a large unsigned-looking value against 0, 1, -1, 3 and a small fraction. They collect all four arithmetic outputs and comparisons; division by zero can terminate the case. Successful numbers include serialized decimal bytes.

The actual [KissOracle.java](../../../../../../../support/KissOracle.java) and [kiss.cpp](../../../../../../../support/kiss.cpp) execute the operations built by [kiss_differential.py](../../../../../../../tools/kiss_differential.py). Their output contains value type, text and underlying HEX/string bytes or number wire bytes. Generic errors are normalized to ERROR; parser errors retain their message. The existing [normal report](../../../../../../../validation/student-linked-evidence/reports/kiss-differential-results.json) records 1,451 cases passed and zero failures; no new run was performed for this lesson.

## Following an exception through the wrapper

The [mapped NumberValueTests lesson](../../../../../../../student-guide/files/test/org/minima/tests/kissvm/values/NumberValueTests.md) now traces 99 active assertions through this wrapper. Its two zero-division assertions exposed a category mismatch in the shared MiniNumber helper. That helper now throws JavaArithmeticException, preserving the Java distinction between `Division undefined` for zero divided by zero and `Division by zero` for a nonzero dividend. NumberValue itself still delegates; the correction belongs where the failed arithmetic occurs.

The four unchanged JUnit methods, 99 native assertions and seven separately labelled error observations pass in both normal and sanitizer builds. All 16 selected regression suites pass, including the 2,890-case consensus comparison. This change does not establish all other MiniNumber exception categories, mutable Java reference behavior, or complete node parity.

## Dependencies and reading map

Direct Minima imports: [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [NumberValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L12), [NumberValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L16), [NumberValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L20), [NumberValue](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L24), [getNumber](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L32), [getValueType](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L37), [isFalse](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L41), [isEqual](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L45), [isLess](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L49), [isLessEqual](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L53), [isMore](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L57), [isMoreEqual](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L61), [add](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L65), [sub](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L69), [mult](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L73), [div](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L77), [toString](../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java#L82).

## Check your understanding

Why might 1.0 and 1 compare equal yet still deserve a wire-representation check?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `46bc38bec53879a43ad5e89f052ac84fe9f73f11ff927c31f7b08a02fa0905ff`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
