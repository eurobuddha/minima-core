# HEX — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/cast/HEX.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/HEX.java) | [src/org/minima/kissvm/functions/cast/HEX.cpp](../../../../../../../../src/org/minima/kissvm/functions/cast/HEX.cpp) |

## Original mapped casting tests

The [casting lab](../../../../../../../new-tests/cast-original-tests.md) records the unchanged pinned test methods and their native counterparts. All four classes pass in normal and ASAN/UBSAN builds, with 87 executed assertions per runtime in total. The existing production casts were reused without modification. These original examples supplement the existing broader comparisons; they do not certify every branch, alias or range boundary.

## What this file does

Convert a Boolean, number, string or HEX value into bytes.

## Follow the logic

1. Require one argument, evaluate it and branch on its KISS type.
2. Map booleans to the single byte 00 or01; preserve HEX bytes.
3. For numbers, reject fractions and negative values before converting the whole nonnegative integer.
4. For strings, use the string’s encoded bytes; return HexValue.

## How the translation is approached

C++ keeps MiniData/BigInt conversion and MiniString encoding instead of using a machine-width integer or a display string for all types.

## What the tests tell us

The unary matrix includes fractions, negatives, values beyond 64 bits, booleans, strings and HEX. It also tests missing and excess arguments.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [HEX](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/HEX.java#L16), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/HEX.java#L21), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/HEX.java#L65), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/HEX.java#L70).

## Check your understanding

Predict HEX([0xFF]) and explain why it differs from HEX(0xFF).

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `3f17098b91c9d37a040f9e2ff0ddf60d69742a40f885043c5e6c977d8e85e637`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
