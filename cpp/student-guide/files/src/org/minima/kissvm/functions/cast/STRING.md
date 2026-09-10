# STRING — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/cast/STRING.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/STRING.java) | [src/org/minima/kissvm/functions/cast/STRING.cpp](../../../../../../../../src/org/minima/kissvm/functions/cast/STRING.cpp) |

## Original mapped casting tests

The [casting lab](../../../../../../../new-tests/cast-original-tests.md) records the unchanged pinned test methods and their native counterparts. All four classes pass in normal and ASAN/UBSAN builds, with 87 executed assertions per runtime in total. The existing production casts were reused without modification. These original examples supplement the existing broader comparisons; they do not certify every branch, alias or range boundary.

## What this file does

Obtain the KISS textual representation of a value.

## Follow the logic

1. Require one argument.
2. Evaluate the expression and call its textual representation.
3. Construct a StringValue around that representation; this is distinct from decoding HEX bytes.

## How the translation is approached

C++ copies an existing StringValue directly and otherwise constructs one from toString().

## What the tests tell us

The shared unary matrix includes every value category and Unicode string [🐼]. Wrong arity and unknown-variable paths are compared too.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [STRING](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/STRING.java#L11), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/STRING.java#L16), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/STRING.java#L25), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/cast/STRING.java#L30).

## Check your understanding

Contrast STRING(0x4142) with ASCII(0x4142).

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `9e53941a3fe30dae7b0bca8d1e3667ce2349f0882bc78f63d49896cdd349b3a9`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
