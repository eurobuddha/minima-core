# SHA3 — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/sha/SHA3.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sha/SHA3.java) | [src/org/minima/kissvm/functions/sha/SHA3.cpp](../../../../../../../../src/org/minima/kissvm/functions/sha/SHA3.cpp) |

## What this file does

Hash raw HEX bytes or encoded string bytes using the shared Crypto SHA3 hash.

## Follow the logic

1. Require one expression and accept only HEX or STRING.
2. Choose raw bytes for HEX, or the string encoding for STRING.
3. Hash those bytes through Crypto and return HEX. No length prefix is added by this wrapper.

## How the translation is approached

C++ reuses MiniData’s sha3 implementation over the selected byte vector.

## What the tests tell us

The unary matrix compares empty/string/HEX inputs, malformed types and wrong argument counts. The result is compared as HEX text; this exercises the common hash implementation indirectly.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [StringValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/StringValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java), [Crypto](../../../../../../../../../upstream-study/src/org/minima/utils/Crypto.java).

Method navigation (declarations with bodies found in this file): [SHA3](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sha/SHA3.java#L16), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sha/SHA3.java#L24), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sha/SHA3.java#L51), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/sha/SHA3.java#L56).

## Check your understanding

Would hashing a serialized MiniData value instead of its raw bytes produce the same digest? Explain why not.

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `0199307c606fc15640b50ee5c1cf25955ae8442723c970fabc9ccefdedd198d3`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
