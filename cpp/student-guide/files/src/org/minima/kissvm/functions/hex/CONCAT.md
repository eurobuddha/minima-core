# CONCAT — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/hex/CONCAT.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/CONCAT.java) | [src/org/minima/kissvm/functions/hex/CONCAT.cpp](../../../../../../../../src/org/minima/kissvm/functions/hex/CONCAT.cpp) |

## What this file does

Join two or more HEX values into one byte sequence.

## Follow the logic

1. Require at least two arguments.
2. Evaluate and type-check each expression in order while adding its byte length to the total. Reject a total above the contract data limit.
3. Copy the sequences in the same order and return HEX.

## How the translation is approached

Java first collects arrays then copies into a final array. C++ appends into a vector while applying the same per-argument type and size checks. Allocation/resource-failure equivalence is not established.

## What the tests tell us

Explicit cases join0xFF/0x00/0x11, reject strings, and reject a single argument. Other size cases exercise related HEX limits but are not claimed as exhaustive CONCAT allocation tests.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [Expression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/Expression.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [CONCAT](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/CONCAT.java#L14), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/CONCAT.java#L19), [isRequiredMinimumParameterNumber](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/CONCAT.java#L64), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/CONCAT.java#L69), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/CONCAT.java#L74).

## Check your understanding

Why does CONCAT require at least two arguments rather than accepting an empty list? Find the source rule.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `4452126a48128ce0a44f2bb9f9307d7c344fed4acf50b3666d0fb236c9ce227e`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
