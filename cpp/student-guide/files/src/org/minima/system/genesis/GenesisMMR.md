# GenesisMMR — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/genesis/GenesisMMR.java](../../../../../../../../upstream-study/src/org/minima/system/genesis/GenesisMMR.java) | [src/org/minima/system/genesis/GenesisMMR.cpp](../../../../../../../src/org/minima/system/genesis/GenesisMMR.cpp) |

## What this file does

Builds the initial one-leaf MMR containing the genesis coin, before the genesis transaction spends it.

## Follow the logic

1. Initialize the inherited MMR at block time zero.
2. Construct GenesisCoin and turn the complete coin into an MMR leaf using its amount as the sum.
3. Add that one entry, then finalizeSet so the inherited MMR computes and caches its finalized structure.
4. Subsequent genesis transaction construction can request the proof to peak for entry zero.

## How the translation is approached

Native code reuses the already mapped MMR, GenesisCoin and MMRData implementations. The constructor is a short composition of these operations rather than a second Merkle-tree implementation.

## What the tests tell us

The genesis oracle returns this MMR’s bytes and JSON, and GenesisTxPoW requests its entry-zero proof. The broader MMR fixtures cover trees with multiple sizes and mutations, but those counts are not independent GenesisMMR tests.

The [Java object oracle](../../../../../../../support/ObjectOracle.java) and [native object harness](../../../../../../../support/objects.cpp) produce matching wire bytes and JSON or explicit derived values. The [object differential driver](../../../../../../../tools/object_differential.py) also re-reads Java-produced bytes, trailing bytes and truncated inputs. The inspected report records 9,244 successful comparisons across the complete object corpus; that number is not a per-class assertion count. Generic `ERROR` comparisons do not establish exact exception-class parity.

## Dependencies and reading map

Direct Minima imports: [Coin](../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MMR](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java), [MMRData](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java).

Method navigation (declarations with bodies found in this file): [GenesisMMR](../../../../../../../../upstream-study/src/org/minima/system/genesis/GenesisMMR.java#L10).

## Check your understanding

Explain why this MMR’s total equals one billion and why its entry-zero proof has no sibling chunks.

## Limits and traceability

 The source map still labels this counterpart partial and parity not_verified. The described fixtures establish their particular observations, not full API equivalence or a working full node.

Pinned Java SHA-256: `bda083b4af36a8f7d13d1b394096f6ba4a3c9675d7e4a41d9da3dd71a789a5e0`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
