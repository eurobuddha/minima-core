# FLOOR — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/number/FLOOR.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/FLOOR.java) | [src/org/minima/kissvm/functions/number/FLOOR.cpp](../../../../../../../../src/org/minima/kissvm/functions/number/FLOOR.cpp) |

## What this file does

Apply floor (largest integer at most the input) to a Minima number.

## Follow the logic

1. Require exactly one argument.
2. Evaluate it as NumberValue, delegating type checks to Contract.
3. Call the corresponding MiniNumber operation and wrap its result. FLOOR(-3.7) is−4.

## How the translation is approached

The C++ wrapper calls the existing MiniNumber method; it does not replace decimal arithmetic with a platform floating-point function.

## What the tests tell us

The numeric-function loop includes0,1,−1,±0.25, variables, HEX/string/Boolean wrong types, a large integer, and zero/two arguments. Numeric results include serialized decimal bytes in the comparison.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Newly translated original test

The [FLOORTests lesson](../../../../../../test/org/minima/tests/kissvm/functions/maths/FLOORTests.md) records three unchanged Java methods and their native counterpart: **30 executed assertions** (11 numeric cases, four constructor/registry assertions and four typed rejections). Both runtimes pass in normal and ASAN/UBSAN builds. Ten selected original-test/mapping suites pass in each build. The native production wrapper and MiniNumber arithmetic were reused unchanged. The original invalid-domain block is empty and its count test checks zero arguments only; do not infer exhaustive coverage from the method names.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [FLOOR](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/FLOOR.java#L11), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/FLOOR.java#L16), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/FLOOR.java#L25), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/FLOOR.java#L30).

## Check your understanding

Which errors arise in this wrapper and which arise inside MiniNumber? Trace one wrong type and one arithmetic edge case.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `1b28bc66bb0b2eb77557f593d841093304a9fecc62e0a203f8f5bea362a7fe04`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
