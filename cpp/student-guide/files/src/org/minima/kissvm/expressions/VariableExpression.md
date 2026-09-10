# VariableExpression — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/expressions/VariableExpression.java](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/VariableExpression.java) | [src/org/minima/kissvm/expressions/VariableExpression.cpp](../../../../../../../src/org/minima/kissvm/expressions/VariableExpression.cpp) |

## What this file does

Resolve a named variable from the current Contract.

## Follow the logic

1. Store the variable name at construction.
2. Charge one instruction, then query Contract.getVariable.
3. Throw the source-specific missing-variable execution error if the result is null; otherwise return the stored Value.
4. Render variable:name for expression display.

## How the translation is approached

C++ preserves shared Value identity and performs an explicit null test. Variable values are looked up at evaluation time, allowing earlier LET statements to change the result.

## What the tests tell us

The fixture defines a, b, t, f, h and s; the driver evaluates those and missing. Script cases also overwrite variables and use them after LET or nested EXEC/FUNCTION.

The [KISS execution driver](../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [Value](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [VariableExpression](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/VariableExpression.java#L18), [getValue](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/VariableExpression.java#L26), [toString](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/VariableExpression.java#L42).

## Check your understanding

When LET x=x+1 runs, which value of x does the right-hand expression read?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `3238ca912974dbd6e72b74c7c7cc1bb8d2781102b67c4fcf28aa937a1fa5592a`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
