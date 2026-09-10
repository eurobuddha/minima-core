# OVERWRITE — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/hex/OVERWRITE.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/OVERWRITE.java) | [src/org/minima/kissvm/functions/hex/OVERWRITE.cpp](../../../../../../../../src/org/minima/kissvm/functions/hex/OVERWRITE.cpp) |

## What this file does

Copy a range of source bytes into a copy of a destination.

## Follow the logic

1. Evaluate source bytes, source position, destination bytes, destination position and length in that order.
2. Copy the destination, so its original value is not changed.
3. Check destination end, then source end, then negative length, using Java int addition. Finally let array-copy range checks reject remaining invalid positions.
4. Return the modified copy.

## How the translation is approached

The native code copies vectors, uses explicit modulo32-bit addition for the source checks and performs bounds checks before std::copy_n to avoid undefined memory access.

## What the tests tell us

Explicit driver cases cover a valid copy, negative source position and too-long copies. Generic runtime diagnostics from range failures are normalized; matching those cases does not establish exact JDK exception text.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java).

Method navigation (declarations with bodies found in this file): [OVERWRITE](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/OVERWRITE.java#L17), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/OVERWRITE.java#L22), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/OVERWRITE.java#L55), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/OVERWRITE.java#L60).

## Check your understanding

Why is checking negative length first a behavioral change when another invalid parameter is also present?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `e7467514464c547898bbd0fdbfe8946f4e8f9162e21f8fca70e9a13359236f21`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
