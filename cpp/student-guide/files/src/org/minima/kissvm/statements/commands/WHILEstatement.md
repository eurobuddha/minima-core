# WHILEstatement — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/statements/commands/WHILEstatement.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/WHILEstatement.java) | [src/org/minima/kissvm/statements/commands/WHILEstatement.cpp](../../../../../../../../src/org/minima/kissvm/statements/commands/WHILEstatement.cpp) |

## What this file does

Repeat a block while a Boolean condition remains true, stopping when the contract result is set.

## Follow the logic

1. Evaluate the condition before entering the loop and require BOOLEAN. The source error text says RETURN, even though this is WHILE.
2. When true, run the body block. If Contract.isSuccessSet becomes true, return before evaluating the condition again.
3. Otherwise evaluate and type-check the condition again; stop once false.

## How the translation is approached

C++ expresses the same sequence as an infinite loop with explicit exits. It retains the source error wording instead of silently correcting it.

## What the tests tell us

The driver includes an initially false loop, an incrementing loop that terminates at five, WHILE TRUE with a RETURN, a non-Boolean condition and an endless loop cut off by instruction limits.

The [KISS execution driver](../../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [Expression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/Expression.java), [Statement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/Statement.java), [StatementBlock](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/StatementBlock.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [WHILEstatement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/WHILEstatement.java#L30), [execute](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/WHILEstatement.java#L36), [toString](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/WHILEstatement.java#L72).

## Check your understanding

Why must a RETURN inside the body prevent evaluation of a now-invalid condition?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `ae2d6f795152a17d2a018917c5e0692bd3e79889248035e6d751a5f54e3d08a1`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
