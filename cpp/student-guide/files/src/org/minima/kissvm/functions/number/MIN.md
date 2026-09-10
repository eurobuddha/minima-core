# MIN — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/number/MIN.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/MIN.java) | [src/org/minima/kissvm/functions/number/MIN.cpp](../../../../../../../../src/org/minima/kissvm/functions/number/MIN.cpp) |

## What this file does

Select the smallest of at least two numeric expressions.

## Follow the logic

1. Require at least two arguments.
2. Evaluate expressions left-to-right and reject any non-number.
3. Keep the first value as the initial candidate; replace it only when a later number is strictly less.
4. Return the selected Value object. Equal values preserve the earlier candidate.

## How the translation is approached

C++ keeps a shared Value pointer as the candidate, retaining the Java aliasing choice rather than synthesizing an unrelated numerical result.

## What the tests tell us

The driver checks empty/one/two/many arguments, a mixed Boolean list and a missing variable. Trace comparison can detect an incorrectly skipped expression.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

The newly mapped [MINTests lesson](../../../../../../../files/test/org/minima/tests/kissvm/functions/maths/MINTests.md) explains three original JUnit methods, 60,000 boundary trials and 120,023 expected successful-path assertions. Production MIN is reused unchanged. Both full builds and all twelve selected suites pass normal and ASAN/UBSAN. Each new test passes 120,023 assertions per runtime. The Java and native tests assert the same boundary property over separately sampled inputs, not identical random streams.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [Expression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/Expression.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [MIN](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/MIN.java#L21), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/MIN.java#L29), [isRequiredMinimumParameterNumber](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/MIN.java#L59), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/MIN.java#L64), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/MIN.java#L69).

## Check your understanding

If the current candidate already seems unbeatable, may evaluation skip the remaining arguments? Explain the observable error case.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `d43110f5a6084aa30deab5a3c3758b637b11b4bb62d4fb819ab0faf71e0a54a5`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
