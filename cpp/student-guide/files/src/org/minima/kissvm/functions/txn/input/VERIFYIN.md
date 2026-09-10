# VERIFYIN — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/txn/input/VERIFYIN.java](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/input/VERIFYIN.java) | [src/org/minima/kissvm/functions/txn/input/VERIFYIN.cpp](../../../../../../../../../src/org/minima/kissvm/functions/txn/input/VERIFYIN.cpp) |

## What this file does

Check several properties of a selected input coin at once.

## Follow the logic

1. Evaluate index, expected address, amount and token ID in source order.
2. Check that the selected input exists. Compare its address and token ID with the expected values.
3. For non-Minima tokens, require metadata and compare the scaled amount.
4. Return the conjunction of all comparisons.

## How the translation is approached

C++ composes the existing Coin/Token comparisons and amount conversion. VERIFYOUT retains the optional fifth parameter and mismatch trace, including the source’s spelling. Passing current cases does not prove arbitrary null/evaluation-order parity.

## What the tests tell us

The driver crosses indices −1..3, amounts0/5/7/12.5/25 and three token selectors. VERIFYOUT adds TRUE/FALSE/wrong-type/excess keep-state arguments. Values, execution state and trace output are compared.

Existing test path: [kissvm_differential.py](../../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [BooleanValue](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [Value](../../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [Coin](../../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [Token](../../../../../../../../../../upstream-study/src/org/minima/objects/Token.java), [Transaction](../../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [VERIFYIN](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/input/VERIFYIN.java#L23), [runFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/input/VERIFYIN.java#L28), [requiredParams](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/input/VERIFYIN.java#L77), [getNewFunction](../../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/txn/input/VERIFYIN.java#L82).

## Check your understanding

Create a case where address matches but amount differs. Then explain how a missing Token can throw instead of simply returning FALSE.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `bcec37bd9af94571724b5da1a9570a5ed88c8d7791d0d51b44c9d5b9eb7a3d69`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
