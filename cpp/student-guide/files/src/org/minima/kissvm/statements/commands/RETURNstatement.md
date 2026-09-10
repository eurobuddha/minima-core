# RETURNstatement — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/statements/commands/RETURNstatement.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/RETURNstatement.java) | [src/org/minima/kissvm/statements/commands/RETURNstatement.cpp](../../../../../../../../src/org/minima/kissvm/statements/commands/RETURNstatement.cpp) |

## What this file does

Set the contract result using a strictly Boolean expression.

## Follow the logic

1. Evaluate the return expression.
2. Reject any value whose type tag is not BOOLEAN.
3. Pass its true/false field to Contract.setRETURNValue; the containing block checks that success-set flag before executing another statement.

## How the translation is approached

C++ retains a separate Boolean type check and writes the shared Contract result. Ordinary C++ truthiness would incorrectly accept numbers or strings.

## What the tests tell us

The driver includes RETURN TRUE, RETURN FALSE, RETURN 1, a missing RETURN expression and multiple RETURN statements, all at varied instruction limits.

The [KISS execution driver](../../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [Expression](../../../../../../../../../upstream-study/src/org/minima/kissvm/expressions/Expression.java), [Statement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/Statement.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [RETURNstatement](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/RETURNstatement.java#L24), [execute](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/RETURNstatement.java#L32), [toString](../../../../../../../../../upstream-study/src/org/minima/kissvm/statements/commands/RETURNstatement.java#L49).

## Check your understanding

Which component actually stops later statements: RETURNstatement or StatementBlock?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `b8261ec401136e946318d7e87fc2aa40d963fa288ef13c723e4b02303586f029`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
