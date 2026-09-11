# EXISTS — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/general/EXISTS.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/EXISTS.java) | [src/org/minima/kissvm/functions/general/EXISTS.cpp](../../../../../../../../src/org/minima/kissvm/functions/general/EXISTS.cpp) |

## What this file does

Look up an array-style variable using a list of numeric indices.

## Follow the logic

1. Require at least one parameter and evaluate the indices in their original order.
2. Require every index to be NUMBER; append its trimmed textual form followed by a comma to a single key.
3. Look up that key in Contract’s variable table. If absent, return FALSE.
4. If present, return TRUE.

## How the translation is approached

C++ builds the same comma-terminated key and reuses Contract’s variable table. GET retains the shared result value; EXISTS returns a Boolean.

## What the tests tell us

The driver writes LET (1 2)=7 and reads GET(1 2), checks EXISTS on missing indices, compares1.0 versus 1 key spelling, replaces a prior value, and supplies wrong index types.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [Expression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/Expression.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [EXISTS](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/EXISTS.java#L14), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/EXISTS.java#L19), [isRequiredMinimumParameterNumber](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/EXISTS.java#L46), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/EXISTS.java#L51), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/EXISTS.java#L56).

## Check your understanding

Write out the exact key for indices 1 and2. Why is evaluating index expressions left-to-right observable?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `9303cecba0c07ff3bc33f78b8f16a5408a2e466a62feb04d7556a20793379c31`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
