# MMREntry — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/mmr/MMREntry.java](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java) | [src/org/minima/objects/mmr/MMREntry.cpp](../../../../../../../src/org/minima/objects/mmr/MMREntry.cpp) |

## What this file does

A position in an MMR: row, entry number and optional hash-and-sum data. It provides the tree arithmetic used to find relatives.

## Follow the logic

1. A position-only constructor represents an empty entry; a constructor with data represents a populated entry. Position comparison checks row and entry number, independently of data.
2. Even entry numbers are left children. The sibling is one position higher for a left child and one lower for a right child.
3. The parent has row + 1 and floor(entry / 2). Children have row - 1 and positions 2*entry and 2*entry + 1.
4. JSON allows null data for an empty entry. Serialization nevertheless expects populated data and writes row as MiniNumber, then entry number and MMRData.
5. Java marks an entry nonempty before reading its fields. A failed read can leave that receiver partly changed; this is distinct from successfully reading a complete entry.

## How the translation is approached

Native data is a shared_ptr, so entry copies can share mutable MMRData. A separate explicit flag represents emptiness, matching the source constructor and read timing. Row arithmetic explicitly uses unsigned addition/subtraction followed by bit_cast to reproduce Java int wraparound without C++ signed-overflow undefined behavior. Static deserialization returns a new populated entry.

## What the tests tell us

The mmrentry branch compares sibling, parent, children and side classification for positive, negative and very large entry numbers and several rows. The object driver also varies fractional entry-number construction; this checks the actual MiniNumber-derived arithmetic rather than assuming every position is a small unsigned integer.

The [Java object oracle](../../../../../../../support/ObjectOracle.java) and [native object harness](../../../../../../../support/objects.cpp) produce matching wire bytes and JSON or explicit derived values. The [object differential driver](../../../../../../../tools/object_differential.py) also re-reads Java-produced bytes, trailing bytes and truncated inputs. The inspected report records 9,244 successful comparisons across the complete object corpus; that number is not a per-class assertion count. Generic `ERROR` comparisons do not establish exact exception-class parity.

## Dependencies and reading map

Direct Minima imports: [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [MMREntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L36), [MMREntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L38), [MMREntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L44), [getEntryNumber](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L51), [getRow](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L55), [getMMRData](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L59), [isEmpty](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L63), [checkPosition](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L67), [checkPosition](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L71), [toJSON](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L80), [toString](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L96), [getParentRow](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L105), [getChildRow](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L109), [isLeft](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L113), [isRight](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L117), [getLeftSibling](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L121), [getRightSibling](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L125), [getSibling](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L129), [getParentEntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L137), [getLeftChildEntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L141), [getRightChildEntry](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L145), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L150), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L163), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java#L170).

## Check your understanding

For row 2 and entry 5, calculate sibling, parent row/entry and both child positions. Then explain why two entries with different data can still have the same position.

## Limits and traceability

The common-interface suite now checks the exact null-data exception class and preserved output prefix. Broader constructor and reference behavior still needs review. The source map still labels this counterpart partial and parity not_verified. The described fixtures establish their particular observations, not full API equivalence or a working full node.

Pinned Java SHA-256: `10da033e42a356f69f4578295ee04dda8c8178f447a55a8c7efef46ebd33b77f`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).

## MMR and signature interface follow-up

The empty flag is now separate from the data pointer. The position-only constructor sets it true; the data constructor sets it false even when explicitly given null. Reading clears it before the first byte, then assigns row, entry number and data as each read completes. Writing null data preserves the row and entry bytes already appended, then throws the source NullPointerException.

The [MMR/signature interface lab](../../../../../../new-tests/streamable-mmr-signatures.md) explains every-prefix reads, partial writes, offsets and actual generic callers. Both complete builds and six affected regressions pass. The expanded interface corpus passes 1,299 comparisons and 6,290 assertions per build across 11 types; these are suite totals, not per-file counts. Full class parity remains unverified.
