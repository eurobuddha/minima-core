# SIGDIG — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/number/SIGDIG.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/SIGDIG.java) | [src/org/minima/kissvm/functions/number/SIGDIG.cpp](../../../../../../../../src/org/minima/kissvm/functions/number/SIGDIG.cpp) |

## What this file does

Round a number to a specified count of significant digits.

## Follow the logic

1. Require precision first and number second.
2. Reject fractional precision, then reject negative precision. Zero passes these guards.
3. Convert precision to int and call MiniNumber.setSignificantDigits.

## How the translation is approached

Native code delegates decimal rounding to MiniNumber and preserves the two validation steps.

## What the tests tell us

The POW/SIGDIG argument product includes fractional, zero, negative and very large precision values. The oracle compares the serialized decimal result too.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [SIGDIG](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/SIGDIG.java#L12), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/SIGDIG.java#L17), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/SIGDIG.java#L36), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/SIGDIG.java#L41).

## Check your understanding

Why should a port preserve zero passing these guards even if the error wording says positive?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `ac1f356e25f1afc710820b166b599ceb597ed532d50719ce0834b84dce3190ce`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## Mapped source-test follow-up

The original SIGDIGTests now has a compiled-test destination with 89 active assertions per runtime. See the [worked lab](../../../../../../../new-tests/power-significant-digits.md) for the tested examples and limitations. Both native and original-test entries pass in normal and sanitizer builds.
