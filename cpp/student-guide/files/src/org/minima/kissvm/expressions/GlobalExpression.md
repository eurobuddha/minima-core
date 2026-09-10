# GlobalExpression — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/expressions/GlobalExpression.java](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/GlobalExpression.java) | [src/org/minima/kissvm/expressions/GlobalExpression.cpp](../../../../../../../src/org/minima/kissvm/expressions/GlobalExpression.cpp) |

## What this file does

Resolve a built-in global such as a block or coin property.

## Follow the logic

1. Retain the global name.
2. Charge one instruction before calling Contract.getGlobal.
3. Return the result and let Contract implement global availability and monotonicity rules.
4. Display global:name.

## How the translation is approached

C++ is the same thin delegation layer. The meaning of a global belongs to Contract, so this wrapper must not cache a previously read value independently.

## What the tests tell us

The ordinary fixture supplies @BLOCK and @CREATED, while rich cases also query @TOTIN, @TOTOUT, @AMOUNT and @COINID. @MISSING exercises a failed lookup; monotonic state is part of the harness output.

The [KISS execution driver](../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [Value](../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [GlobalExpression](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/GlobalExpression.java#L19), [getValue](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/GlobalExpression.java#L24), [toString](../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/GlobalExpression.java#L33).

## Check your understanding

Why might reading a block-dependent global affect whether a script result can safely be reused?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `f4acbb3f3f106bafe058a70ec7c501c52593bb744b4df1e9758621ec7267e9f3`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
