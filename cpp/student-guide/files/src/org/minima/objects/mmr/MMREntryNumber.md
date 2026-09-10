# MMREntryNumber — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/mmr/MMREntryNumber.java](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java) | [src/org/minima/objects/mmr/MMREntryNumber.cpp](../../../../../../../src/org/minima/objects/mmr/MMREntryNumber.cpp) |

## What this file does

An unbounded decimal position value used for MMR tree arithmetic. Unlike MiniNumber, it does not impose the monetary range or 64-digit precision limit.

## Follow the logic

1. Store a BigDecimal with unlimited math context. Constructors permit large values and decimal scales; the type itself does not enforce an integer or nonnegative position invariant.
2. increment/decrement add or subtract one, mult2 doubles the value, div2 halves it exactly, and floor rounds toward negative infinity. For an odd integer, halving creates a fractional value before a caller applies floor.
3. modulo uses BigDecimal remainder semantics. Numeric comparison ignores representational scale; toString strips trailing zeros and prints plain decimal text.
4. Write the decimal scale as a complete MiniNumber, then wrap the signed unscaled BigInteger bytes in MiniData. This differs from MiniNumber’s compact one-byte scale/length representation.
5. Read scale with MiniNumber.getAsInt, read the unscaled byte array and reconstruct an unlimited BigDecimal. Empty unscaled bytes fail BigInteger construction.

## How the translation is approached

C++ reuses the BigInt building block but stores an independent int32 scale and does not call MiniNumber limit checks on the position. Halving an odd unscaled integer multiplies by five and increments scale; doubling and scale alignment remain exact. Additional native dividePositive/multiplyDecimal32 helpers implement rounded chain-weight arithmetic and are adapters beyond this Java file’s API.

## What the tests tell us

The entry branch tests unscaled values including negatives, 2^128 and 2^256+1 at scales -10, -1, 0, 1, 2 and 15. It compares wire bytes and derived arithmetic. The mmrentry branch connects positions to parent/sibling/child calculations. Arbitrarily huge allocations and every signed scale boundary remain outside this finite corpus.

Read the actual [Java object oracle](../../../../../../../support/ObjectOracle.java), [native harness](../../../../../../../support/objects.cpp) and [fixture driver](../../../../../../../tools/object_differential.py). The inspected full-object report has 9,244 matching observations with zero failures. That is a corpus total; generic ERROR matching does not prove exact failure types.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [MMREntryNumber](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L30), [MMREntryNumber](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L34), [MMREntryNumber](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L38), [MMREntryNumber](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L42), [getBigDecimal](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L46), [modulo](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L50), [floor](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L54), [increment](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L58), [decrement](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L62), [div2](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L66), [mult2](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L70), [compareTo](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L75), [isEqual](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L79), [isLess](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L83), [isLessEqual](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L87), [isMore](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L91), [isMoreEqual](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L95), [toString](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L100), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L108), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L118), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java#L130).

## Check your understanding

For entry number -3, what are div2 and floor(div2)? Why would ordinary integer division toward zero give the wrong parent position?

## Limits and traceability

Exact BigDecimal-returning APIs are adapted rather than exposed literally. Empty BigInteger and scale-overflow errors use native exception categories; generic comparison failures do not establish those categories. The source-map status remains partial and full parity not_verified.

Pinned Java SHA-256: `2b9839210ee877ae25e13d98556fd1a845c572ef2dfb5b372a374d3d7fb09891`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
