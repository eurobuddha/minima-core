# BOOL — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/cast/BOOL.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/BOOL.java) | [src/org/minima/kissvm/functions/cast/BOOL.cpp](../../../../../../../../src/org/minima/kissvm/functions/cast/BOOL.cpp) |

## Original mapped casting tests

The [casting lab](../../../../../../../new-tests/cast-original-tests.md) records the unchanged pinned test methods and their native counterparts. All four classes pass in normal and ASAN/UBSAN builds, with 87 executed assertions per runtime in total. The existing production casts were reused without modification. These original examples supplement the existing broader comparisons; they do not certify every branch, alias or range boundary.

## What this file does

Convert any supported KISS value into a Boolean.

## Follow the logic

1. Evaluate the single expression before deciding its type.
2. Booleans keep their value; numbers and hexadecimal integers are false only when numerically zero.
3. Strings are false only when their exact text is FALSE. In particular an empty string and lowercase false are true.
4. Reject unsupported value types and return a BooleanValue.

## How the translation is approached

A C++ switch replaces Java instanceof-style casts while retaining the four separate conversion rules.

## What the tests tell us

The driver explicitly includes TRUE/FALSE, 0, nonzero numbers, [FALSE], [false], [], 0x00 and 0xFFFF, then compares typed results and failures.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [BOOL](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/BOOL.java#L15), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/BOOL.java#L20), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/BOOL.java#L62), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/BOOL.java#L67).

## Check your understanding

Why would using C++ string.empty() as this conversion produce a consensus difference?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `65e77779b82b92ef49fda2333a8556f88d1e1fbb00e2c3326f287c52acaeabbc`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
