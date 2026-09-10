# Statement — KISS core lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/statements/Statement.java](../../../../../../../../upstream-study/src/org/minima/kissvm/statements/Statement.java) | [src/org/minima/kissvm/statements/Statement.cpp](../../../../../../../src/org/minima/kissvm/statements/Statement.cpp) |

## What this file does

Define the executable statement interface used by a block.

## Follow the logic

1. The Java interface declares execute(Contract), which may throw ExecutionException. It has no executable body.
2. Concrete statements evaluate expressions and mutate the supplied Contract. StatementBlock supplies sequencing, accounting and trace calls around them.

## How the translation is approached

C++ uses an abstract base with a virtual destructor, execute and a required toString method. Shared StatementPtr values let parsed blocks own polymorphic statements. The additional explicit toString contract corresponds to Java Object dispatch used by tracing.

## What the tests tell us

All script cases dispatch through StatementBlock to concrete statements. This validates representative implementations of the interface, not an abstract interface in isolation.

The [KISS execution driver](../../../../../../../tools/kissvm_differential.py), [Java oracle](../../../../../../../support/KissVMOracle.java) and [native harness](../../../../../../../support/kissvm.cpp) compare actual execution, state, instruction counts and selected traces. See [the test-method lesson](../../../../../../evidence/kiss-builtins.md) for fixture construction and normalization limits. The existing normal report records 4,471 comparisons passed and zero failures; this is not a fresh run or a per-class test count.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java).

## Check your understanding

What goes wrong if a polymorphic C++ base is deleted through a pointer without a virtual destructor?

## Limits and traceability

This lesson explains the inspected bodies and stated test scenarios, not complete API or whole-node parity.  Generic runtime normalization can conceal differences in exception class, text or trace. All file parity remains not_verified.

Pinned Java SHA-256: `936fa86ddc7cedb9248cd48b0e45cfe920ff71f66320e09bc6e14e853d2bf174`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
