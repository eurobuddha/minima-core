# SAMESTATE — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/state/SAMESTATE.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/state/SAMESTATE.java) | [src/org/minima/kissvm/functions/state/SAMESTATE.cpp](../../../../../../../../src/org/minima/kissvm/functions/state/SAMESTATE.cpp) |

## What this file does

Compare old and current state text across an inclusive port range.

## Follow the logic

1. Require two numeric endpoints; reject negative start or end before start.
2. For each port from start through end, read previous and current state and compare their toString values.
3. On the first difference, emit a trace message and return FALSE. If every port matches, return TRUE.

## How the translation is approached

C++ uses an int64 loop variable to avoid undefined overflow at the32-bit endpoint while preserving ordinary inclusive traversal. Extreme-loop/API equivalence is not established by current cases.

## What the tests tell us

Explicit cases include ranges0..1,−1..0,1..0 and2..2. The fixture deliberately has unequal old/current values, exercising early FALSE, invalid range and missing state.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [SAMESTATE](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/state/SAMESTATE.java#L19), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/state/SAMESTATE.java#L24), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/state/SAMESTATE.java#L53), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/state/SAMESTATE.java#L58).

## Check your understanding

Why can moving all state lookups ahead of the comparison change behavior when a later port is absent?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `839452afcf2f14323f60fe7b1395ca3e277c53c129c2184e2383a2aa84058369`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
