# MinimaParseException — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/exceptions/MinimaParseException.java](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java) | [src/org/minima/kissvm/exceptions/MinimaParseException.cpp](../../../../../../../src/org/minima/kissvm/exceptions/MinimaParseException.cpp) |

## What this file does

Identify a failure while building a valid KISS program.

## Follow the logic

1. Receive the error string in the constructor.
2. Pass it unchanged to the superclass constructor. There is no separate algorithm or recovery policy in this file.
3. Let callers catch this type or a superclass; the distinction changes whether an error is propagated, wrapped or recorded in Contract.

## How the translation is approached

C++ preserves the three-class inheritance relationship, with MinimaException deriving from std::runtime_error. Rendering a Java-qualified exception name is handled by surrounding exceptionString logic, not by duplicating it in every constructor.

## What the tests tell us

The execution driver contains malformed token/statement/function-arity cases and runtime failures such as ASSERT 0 or an out-of-range built-in. The actual oracle catches MinimaException separately from generic Exception, so the exception family affects serialized observations. There is no dedicated constructor-only assertion claimed here.

The [KISS execution driver](../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Method navigation (declarations with bodies found in this file): [MinimaParseException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/MinimaParseException.java#L11).

## Check your understanding

Explain why catching every failure as a generic runtime error would lose information even though each class stores only a message.

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity. Complete Java checked-exception semantics and every native failure category are not established. Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `b508930bb67b41871996b52998c1aabd8d16de4d5376fe7baec9611537b74fa5`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
