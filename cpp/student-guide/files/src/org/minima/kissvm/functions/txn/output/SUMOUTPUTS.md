# SUMOUTPUTS — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/txn/output/SUMOUTPUTS.java](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/output/SUMOUTPUTS.java) | [src/org/minima/kissvm/functions/txn/output/SUMOUTPUTS.cpp](../../../../../../../../../src/org/minima/kissvm/functions/txn/output/SUMOUTPUTS.cpp) |

## What this file does

Sum amounts of output coins matching one token ID.

## Follow the logic

1. Require one HEX token identifier.
2. Start at MiniNumber.ZERO and visit every output coin. Ignore other token IDs.
3. For matching Minima coins add the raw amount; for other matching tokens add the amount scaled through attached Token metadata.
4. Return the final NumberValue.

## How the translation is approached

C++ uses existing decimal addition and Token scaling. The current optional.value() on missing metadata may yield a generic native error; the harness normalization means exact exception-class parity is not established.

## What the tests tell us

The driver checks Minima 0x00, the fixture’s scaled token variable tok,0x77 with missing metadata, and unmatched 0x88. This distinguishes raw amounts, scaled amounts, error and zero total.

Existing test path: [kissvm_differential.py](../../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [NumberValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [Coin](../../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [Token](../../../../../../../../../../upstream-study/src/org/minima/objects/Token.java), [Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [SUMOUTPUTS](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/output/SUMOUTPUTS.java#L18), [runFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/output/SUMOUTPUTS.java#L23), [requiredParams](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/output/SUMOUTPUTS.java#L62), [getNewFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/output/SUMOUTPUTS.java#L67).

## Check your understanding

Why would summing raw stored values produce a wrong answer for a non-Minima token?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `f211c2fde96b0187574633ebe7253bb7ac484b524e237b08954d85ed676f8638`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
