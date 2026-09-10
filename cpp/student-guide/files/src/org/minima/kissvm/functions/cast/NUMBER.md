# NUMBER — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/cast/NUMBER.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/NUMBER.java) | [src/org/minima/kissvm/functions/cast/NUMBER.cpp](../../../../../../../../src/org/minima/kissvm/functions/cast/NUMBER.cpp) |

## Original mapped casting tests

The [casting lab](../../../../../../../new-tests/cast-original-tests.md) records the unchanged pinned test methods and their native counterparts. All four classes pass in normal and ASAN/UBSAN builds, with 87 executed assertions per runtime in total. The existing production casts were reused without modification. These original examples supplement the existing broader comparisons; they do not certify every branch, alias or range boundary.

## What this file does

Convert a supported KISS value into Minima decimal arithmetic.

## Follow the logic

1. Evaluate one expression.
2. Map Boolean true/false to 1/0 and interpret HEX as an unsigned integer.
3. Parse string contents through NumberValue’s decimal parser, or copy an existing number.
4. Reject other type tags.

## How the translation is approached

The native result wraps MiniNumber; it does not use double, so the existing decimal limits and wire representation remain relevant.

## What the tests tell us

The unary matrix includes nonnumeric strings, very large HEX, fractions, TRUE/FALSE and wrong arities; the harness also compares serialized MiniNumber bytes.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [NUMBER](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/NUMBER.java#L22), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/NUMBER.java#L27), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/NUMBER.java#L61), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/NUMBER.java#L66).

## Check your understanding

Why is numeric equality alone weaker than comparing the decimal wire bytes?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `40f0ba5319c09132ed151025e4f9c8fe71aa35b7544a2ae5199463450ceaeb3e`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
