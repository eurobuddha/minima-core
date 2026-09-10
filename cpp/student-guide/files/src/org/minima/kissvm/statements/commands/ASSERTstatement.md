# ASSERTstatement — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/statements/commands/ASSERTstatement.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/ASSERTstatement.java) | [src/org/minima/kissvm/statements/commands/ASSERTstatement.cpp](../../../../../../../../src/org/minima/kissvm/statements/commands/ASSERTstatement.cpp) |

## What this file does

Stop the contract with failure when a required Boolean condition is false.

## Follow the logic

1. Evaluate the assertion expression.
2. Reject a non-Boolean with the source ASSERT type error.
3. If false, call Contract.setRETURNValue(false). If true, leave the return state unchanged so execution can continue.

## How the translation is approached

The native statement stores a shared expression and reproduces the conditional state update. It does not throw merely because a valid Boolean assertion is false.

## What the tests tell us

Explicit scripts include ASSERT FALSE RETURN TRUE, ASSERT TRUE RETURN TRUE and ASSERT 0 at several instruction limits. Comparing success-set and exception flags distinguishes ordinary false from a type error.

The [KISS execution driver](../../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [Expression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/Expression.java), [Statement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/Statement.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [ASSERTstatement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/ASSERTstatement.java#L24), [execute](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/ASSERTstatement.java#L29), [toString](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/ASSERTstatement.java#L48).

## Check your understanding

Why is ASSERT FALSE not equivalent to throwing an ExecutionException?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `a209a6cde45655c8810bc8f9bfc3e56cb56b407c3cf9226774f678f56aa2bef5`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
