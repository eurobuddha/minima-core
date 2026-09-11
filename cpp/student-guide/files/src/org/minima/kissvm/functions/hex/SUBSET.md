# SUBSET — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/hex/SUBSET.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/SUBSET.java) | [src/org/minima/kissvm/functions/hex/SUBSET.cpp](../../../../../../../../src/org/minima/kissvm/functions/hex/SUBSET.cpp) |

## What this file does

Extract a half-open range of HEX bytes.

## Follow the logic

1. Evaluate start and end, then calculate end−start with Java int semantics.
2. Reject negative or excessive result length before evaluating the HEX array.
3. Check both endpoints against the array bounds and copy [start,end). An empty range is permitted.

## How the translation is approached

C++ explicitly models32-bit subtraction before using checked vector iterators.

## What the tests tell us

The driver takes a product of negative, zero, normal and extreme32-bit endpoints against 0xAABBCC. It compares successful values and ordered failures.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [SUBSET](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/SUBSET.java#L16), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/SUBSET.java#L21), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/SUBSET.java#L57), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/SUBSET.java#L62).

## Check your understanding

Explain why SUBSET(1 1 0xAABBCC) is valid even though SETLEN(0 ...) is not.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `e1f4455a9342d02db4fb0fb569f3393d9c7b7949a1d7aa3c5375e94e8b43f105`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
