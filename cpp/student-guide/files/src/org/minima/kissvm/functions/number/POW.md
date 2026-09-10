# POW — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/number/POW.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/POW.java) | [src/org/minima/kissvm/functions/number/POW.cpp](../../../../../../../../src/org/minima/kissvm/functions/number/POW.cpp) |

## What this file does

Raise a number to an integer exponent; the exponent is the first argument.

## Follow the logic

1. Require two numeric arguments: exponent first, base second.
2. Reject a fractional exponent and any exponent whose absolute value is at least 1024.
3. Convert the exponent to int and delegate exponentiation to MiniNumber. Negative integral exponents reach that arithmetic implementation.

## How the translation is approached

The C++ code preserves parameter order and both guards; it uses MiniNumber.pow instead of std::pow.

## What the tests tell us

The driver varies first arguments −1024/−2/0/0.5/1/2/64/1023/1024/4294967296 against several bases, exercising boundary and fractional exponents.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [POW](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/POW.java#L12), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/POW.java#L17), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/POW.java#L38), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/POW.java#L43), [main](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/number/POW.java#L47).

## Check your understanding

Predict POW(3 2). Why might a reader expecting pow(base,exponent) port this incorrectly?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `9f1980e13136d4d951a005a7663cd6c864a39aec048d4ee479a73d08bcb571f9`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## Mapped source-test follow-up

The original POWTests now has a compiled-test destination with 25 active assertions per runtime. See the [worked lab](../../../../../../../new-tests/power-significant-digits.md) for the tested examples and limitations. Both native and original-test entries pass in normal and sanitizer builds.
