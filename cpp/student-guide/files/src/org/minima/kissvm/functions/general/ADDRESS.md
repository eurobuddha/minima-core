# ADDRESS — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/general/ADDRESS.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/ADDRESS.java) | [src/org/minima/kissvm/functions/general/ADDRESS.cpp](../../../../../../../../src/org/minima/kissvm/functions/general/ADDRESS.cpp) |

## What this file does

Calculate the address associated with a script string.

## Follow the logic

1. Require exactly one string argument.
2. Construct the shared Address object from that script text.
3. Return its address bytes as HEX.

## How the translation is approached

Both versions delegate address derivation to Address. The C++ built-in is a thin wrapper and does not introduce a second hashing algorithm.

## What the tests tell us

The unary matrix supplies strings, Unicode, empty strings and incorrect types. Output values are compared with Java through the real expression evaluator.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [Address](../../../../../../../../../upstream-study/src/org/minima/objects/Address.java).

Method navigation (declarations with bodies found in this file): [ADDRESS](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/ADDRESS.java#L13), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/ADDRESS.java#L18), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/ADDRESS.java#L31), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/general/ADDRESS.java#L36).

## Check your understanding

Where would you inspect the serialization and hash domain if two address outputs differed?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `06571428f3cf40f0079282440c254df352a4b492601181b72eba7b5dc8c3afb8`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
