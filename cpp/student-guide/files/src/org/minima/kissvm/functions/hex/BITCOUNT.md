# BITCOUNT — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/hex/BITCOUNT.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/BITCOUNT.java) | [src/org/minima/kissvm/functions/hex/BITCOUNT.cpp](../../../../../../../../src/org/minima/kissvm/functions/hex/BITCOUNT.cpp) |

## What this file does

Count the set bits in a byte sequence.

## Follow the logic

1. Evaluate exactly one HEX argument.
2. Visit every byte and count its one bits. Java masks with 0xFF before looking up a256-entry table.
3. Sum the counts and return a NumberValue.

## How the translation is approached

C++ indexes the source BITSPERBYTE table with uint8_t values and preserves the signed 32-bit total through unsigned accumulation and an explicit bit conversion.

## What the tests tell us

The unary matrix includes 0x00, 0x80 and 0xFFFF (zero, one and sixteen set bits), plus invalid types/counts.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [NumberValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/NumberValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [BITCOUNT](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/BITCOUNT.java#L21), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/BITCOUNT.java#L29), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/BITCOUNT.java#L43), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/BITCOUNT.java#L48), [totalBits](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/BITCOUNT.java#L52).

## Check your understanding

Why must Java mask a signed byte before using it as an array index?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `5411d870c7fd6da2a1866575978921ce77761017be7cc3b712205f2acb898316`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## Published table and mapped source test

The native class now exposes the pinned 256-entry BITSPERBYTE table and totalBits reads its mutable entries. Java final fixes an array reference, not its elements. Unsigned native accumulation preserves Java signed 32-bit wrapping. The [worked lesson](../../../../../../../new-tests/bit-counting.md) separates 34 successful assertion calls, 256 table predicates and supplemental mutation observations. The original/native test entries, supplemental table observations and KISS VM differential suite pass in normal and sanitizer builds.
