# SUBSTR — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/string/SUBSTR.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/string/SUBSTR.java) | [src/org/minima/kissvm/functions/string/SUBSTR.cpp](../../../../../../../../src/org/minima/kissvm/functions/string/SUBSTR.cpp) |

## What this file does

Extract a half-open UTF-16 substring.

## Follow the logic

1. Read start and end and compute Java int end−start. Reject a negative length.
2. Evaluate the string, then reject a negative start or an end beyond Java String.length().
3. Extract [start,end) and construct StringValue. A range may split a surrogate pair; construction then applies the shared UTF-8 round trip.

## How the translation is approached

C++ indexes std::u16string and preserves modulo32-bit subtraction; explicit extra range checks prevent undefined access.

## What the tests tell us

The endpoint cross-product runs on [a🐼b]. Additional cases join two individually split halves, compare a half with [?], and measure/encode that half, exposing the StringValue conversion boundary.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [SUBSTR](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/string/SUBSTR.java#L16), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/string/SUBSTR.java#L21), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/string/SUBSTR.java#L48), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/string/SUBSTR.java#L53).

## Check your understanding

Why can extracting each half of an emoji separately and concatenating them fail to reproduce the original emoji?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `1a70a3530babb4becad1ceae07354dff63da8962982f82e096acef75c489548a`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
