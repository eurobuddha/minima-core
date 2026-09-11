# SETLEN — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/hex/SETLEN.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/SETLEN.java) | [src/org/minima/kissvm/functions/hex/SETLEN.cpp](../../../../../../../../src/org/minima/kissvm/functions/hex/SETLEN.cpp) |

## What this file does

Resize HEX while keeping the least-significant/rightmost bytes.

## Follow the logic

1. Read the requested length first and reject values below 1 or above MAX_DATA_SIZE.
2. Evaluate the HEX argument only after that check.
3. For shrinking, keep the rightmost newLength bytes. For growing, left-pad with zero bytes.
4. Return the resized HEX value.

## How the translation is approached

C++ allocates a zero-initialized vector and copies min(oldLength,newLength) bytes from the end of each array.

## What the tests tell us

LEN(SETLEN(...)) is exercised at −1,0,1,2,4,65536 and65537; the driver compares success or source error as well as instruction state.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [SETLEN](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/SETLEN.java#L16), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/SETLEN.java#L21), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/SETLEN.java#L55), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/SETLEN.java#L60).

## Check your understanding

Give the outputs of SETLEN(2 0xAABBCC) and SETLEN(4 0xAABBCC).

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `eccd2684de22337787b2b0b9c38b9a92d828528c28bed736e1b03f0a39e02274`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
