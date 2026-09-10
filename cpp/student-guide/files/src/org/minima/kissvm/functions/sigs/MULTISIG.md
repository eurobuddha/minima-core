# MULTISIG — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/sigs/MULTISIG.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/MULTISIG.java) | [src/org/minima/kissvm/functions/sigs/MULTISIG.cpp](../../../../../../../../src/org/minima/kissvm/functions/sigs/MULTISIG.cpp) |

## What this file does

Count matching signature-key arguments until a requested threshold is reached.

## Follow the logic

1. Require a threshold and at least one key. Convert the threshold to int and reject negatives.
2. Visit each key expression in order and count each successful Contract signature lookup. The source does not deduplicate repeated key arguments.
3. Break after checking the current key if the threshold has been reached; return whether enough matches were found. Even threshold 0 can evaluate the first key.

## How the translation is approached

C++ preserves the count and post-iteration early-break position.

## What the tests tell us

Explicit rich cases check two distinct keys, duplicate0xCA keys and a missing trailing expression after a satisfied threshold. Other cases use0/negative thresholds and a numeric key.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../build/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [BooleanValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/BooleanValue.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [MULTISIG](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/MULTISIG.java#L12), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/MULTISIG.java#L17), [isRequiredMinimumParameterNumber](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/MULTISIG.java#L53), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/MULTISIG.java#L58), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sigs/MULTISIG.java#L63).

## Check your understanding

Does MULTISIG(2 0xCA 0xCA) require two different keys in this source? Locate the operation that answers this.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `dcfddde4302fc5714c91fc2278abac73ebbeb3d0fb210c3406607bc5990e7dc5`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
