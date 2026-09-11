# BITSET — KISS built-in lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/kissvm/functions/hex/BITSET.java](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/BITSET.java) | [src/org/minima/kissvm/functions/hex/BITSET.cpp](../../../../../../../../src/org/minima/kissvm/functions/hex/BITSET.cpp) |

## What this file does

Change one bit using Java BitSet byte ordering.

## Follow the logic

1. Require three arguments. Evaluate HEX data and convert the index to a Java-style int.
2. Reject an index below 0 or above byteLength×8−1. Bit 0 is the least significant bit of the first byte.
3. Evaluate the Boolean setting only after index validation, then set or clear the bit. Remove trailing zero bytes when converting the BitSet back to HEX.

## How the translation is approached

C++ uses byte index bit/8 and mask 1<<(bit%8). A loop removes zero bytes at the back to match BitSet.toByteArray; retaining the original length would be wrong.

## What the tests tell us

The driver combines several byte orders (0x0001 versus 0x0100), empty HEX, indices −1/0/7/8/15/16/4294967296, and, for BITSET, TRUE/FALSE/invalid numeric flags. Values, errors and traces are compared.

Existing test path: [kissvm_differential.py](../../../../../../../../tools/kissvm_differential.py) constructs the cases; [KissVMOracle.java](../../../../../../../../support/KissVMOracle.java) executes the pinned Java and [kissvm.cpp](../../../../../../../../support/kissvm.cpp) executes the native implementation. The comparison includes typed value/text, decimal bytes for number values, parse/success/error flags, instruction count, stack depth, monotonic flag, variables and trace. Generic runtime diagnostics are normalized to ERROR, so their exact exception classes/messages are not proved.

Backfill snapshot: [existing normal report](../../../../../../../../validation/hash-writers-work-in-progress/normal/kissvm-differential-results.json) records 4,471 comparisons passed and 0 failures. This guide did not rerun them. See [the shared test lesson](../../../../../../../evidence/kiss-builtins.md) for the fixture and the limits.

## Dependencies and reading map

Direct Minima imports: [Contract](../../../../../../../../../upstream-study/src/org/minima/kissvm/Contract.java), [ExecutionException](../../../../../../../../../upstream-study/src/org/minima/kissvm/exceptions/ExecutionException.java), [MinimaFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/MinimaFunction.java), [HexValue](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/HexValue.java), [Value](../../../../../../../../../upstream-study/src/org/minima/kissvm/values/Value.java).

Method navigation (declarations with bodies found in this file): [BITSET](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/BITSET.java#L16), [runFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/BITSET.java#L24), [requiredParams](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/BITSET.java#L54), [getNewFunction](../../../../../../../../../upstream-study/src/org/minima/kissvm/functions/hex/BITSET.java#L59).

## Original test mapping

The mapped BITSETTests now runs all three unchanged Java methods and their native counterpart in both normal and sanitizer builds: 271 successful assertions per runtime. Its independently built BitSet inputs establish least-significant-bit-first indexing and empty-array output when the final set bit is cleared. See the [bit experiment](../../../../../../../new-tests/bit-get-set.md). The production function was reused unchanged.

## Check your understanding

Calculate BITSET(0x0100 0 FALSE). Why may the resulting length shrink to zero?

## Limits and traceability

A corresponding file and a passing finite suite do not establish complete 1:1 behavior. Full node startup and native H2/MVStore storage remain unfinished. This lesson explains the inspected built-in body and the stated KISS cases; it does not certify all public API, allocator, locale or exception semantics.

Pinned Java SHA-256: `fc295eadcfdfc4345653913e01b76752ac08b42f029ac951a270fc345ca922c3`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
