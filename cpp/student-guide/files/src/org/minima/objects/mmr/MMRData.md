# MMRData — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/mmr/MMRData.java](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java) | [src/org/minima/objects/mmr/MMRData.cpp](../../../../../../../src/org/minima/objects/mmr/MMRData.cpp) |

## What this file does

A hash-and-sum commitment used at every MMR node. The hash commits to structure and contents; the value tracks the accumulated amount.

## Follow the logic

1. Create a leaf by hashing serialized MiniNumber.ZERO, the serialized object, and its sum. The zero prefix distinguishes a leaf from an internal node.
2. Create a parent by adding left and right values, then hashing MiniNumber.ONE, left hash, right hash and the combined sum. Left/right order matters even when addition gives the same sum.
3. Equality requires both hash and value to match. Comparing only the hash would omit part of the object’s explicit equality contract.
4. The unspendable flag is mutable in-memory metadata. It is absent from serialization and JSON, so decoding a new object resets it to its default, while reading into an existing object leaves it unchanged.
5. Write the hash using the hash-specific MiniData encoding, followed by MiniNumber value. Read the same order.

## How the translation is approached

The native leaf helper is a C++ template accepting serializable objects; both leaf and parent reuse Crypto::hashAllObjects. It preserves the unspendable flag as a public bool with accessor methods, separate from wire data.

## What the tests tell us

The leaf branch uses script MiniString data and selected sums; proof cases repeatedly combine leaves and compare computed roots. Coin proofs also commit complete serialized coins. These observations exercise both domains and sum propagation, but do not prove all possible object serializers are correct.

The [Java object oracle](../../../../../../../support/ObjectOracle.java) and [native object harness](../../../../../../../support/objects.cpp) produce matching wire bytes and JSON or explicit derived values. The [object differential driver](../../../../../../../tools/object_differential.py) also re-reads Java-produced bytes, trailing bytes and truncated inputs. The inspected report records 9,244 successful comparisons across the complete object corpus; that number is not a per-class assertion count. Generic `ERROR` comparisons do not establish exact exception-class parity.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Crypto](../../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [Streamable](../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [CreateMMRDataLeafNode](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L30), [CreateMMRDataParentNode](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L38), [MMRData](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L52), [MMRData](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L54), [getData](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L59), [getValue](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L63), [setUnspendable](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L67), [isUnspendable](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L71), [isEqual](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L75), [toJSON](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L79), [toString](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L87), [writeDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L92), [readDataStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L98), [ReadFromStream](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java#L103).

## Check your understanding

Construct two conceptual leaves with sums 2 and 3. Explain why swapping them preserves the root sum 5 but usually changes the root hash.

## Limits and traceability

 The source map still labels this counterpart partial and parity not_verified. The described fixtures establish their particular observations, not full API equivalence or a working full node.

Pinned Java SHA-256: `fea38cf0f064be9b07c1dd2f2f51a52ee369a303ece9b2b19387146f14e0088a`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).

## MMR and signature interface follow-up

The in-place reader assigns the decoded hash before reading the sum. If that later read fails, the new hash remains and the previous sum survives. The unspendable flag stays unchanged; it is not part of the stream. The writer appends the hash and then the sum using the existing codecs.

The [MMR/signature interface lab](../../../../../../new-tests/streamable-mmr-signatures.md) explains every-prefix reads, partial writes, offsets and actual generic callers. Both complete builds and six affected regressions pass. The expanded interface corpus passes 1,299 comparisons and 6,290 assertions per build across 11 types; these are suite totals, not per-file counts. Full class parity remains unverified.
