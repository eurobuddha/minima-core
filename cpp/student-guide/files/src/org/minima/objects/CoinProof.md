# CoinProof — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/CoinProof.java](../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java) | [src/org/minima/objects/CoinProof.cpp](../../../../../../src/org/minima/objects/CoinProof.cpp) |

## What this file does

Packages the complete coin and the MMR path needed to locate its commitment. The path is evidence to be checked elsewhere; constructing this wrapper does not authenticate a coin.

## Follow the logic

1. Keep a Coin and an MMRProof together so that a verifier can reconstruct the exact committed leaf.
2. Build that leaf with CreateMMRDataLeafNode(coin, coin.amount). Both the serialized coin and its amount contribute to the commitment; the amount also becomes the MMR sum.
3. Render separate coin and proof JSON fields. Serialize the coin first and the proof second, with no extra wrapper tag.
4. Read the coin completely, then read the proof. The Java convenience converter logs an IOException and returns null; ordinary reading propagates errors.

## How the translation is approached

Java stores object references. Native CoinProof, Coin and outer MMRProof now use shared field storage. A copied wrapper handle observes replacement of the wrapper's coin or proof; a separately retained old coin/proof keeps its previous object alive. The in-place reader assigns each child only after that child finishes decoding. The converter catches only I/O errors, calls the actual logger and returns an empty optional. Nested proof-chunk and value-object identity remain incomplete.

## What the tests tell us

The coinproof fixture uses coin indices 0 through 31, varying state, spent status, entry number and token metadata, and compares the wrapped bytes, JSON and derived leaf. Its proof starts at block time 42 with an empty chain, so this fixture alone does not demonstrate an authenticated nonempty path.

The [Java object oracle](../../../../../../support/ObjectOracle.java) and [native object harness](../../../../../../support/objects.cpp) produce matching wire bytes and JSON or explicit derived values. The [object differential driver](../../../../../../tools/object_differential.py) also re-reads Java-produced bytes, trailing bytes and truncated inputs. The inspected report records 9,464 successful comparisons across the complete object corpus; that number is not a per-class assertion count. Generic `ERROR` comparisons do not establish exact exception-class parity.

The [coin ownership lab](../../../../../new-tests/coin-ownership.md) adds deliberate coin mutations through the wrapper and its witness owner, including after container growth, removal and clearing. It also compares the recalculated MMR leaf. See that lab for the baseline and current validation checkpoint; that earlier experiment does not certify MMRProof identity or converter errors. The subsequent [reader lab](../../../../../new-tests/coin-streams.md) tests outer proof ownership, partially completed reads and actual converter logging separately.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MMRData](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java), [MMRProof](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java), [MinimaLogger](../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONObject](../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [CoinProof](../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java#L27), [CoinProof](../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java#L29), [getCoin](../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java#L34), [getMMRProof](../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java#L38), [getMMRData](../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java#L42), [toJSON](../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java#L54), [convertMiniDataVersion](../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java#L64), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java#L85), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java#L91), [ReadFromStream](../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java#L96).

## Check your understanding

If a caller changes the coin’s spent flag after constructing a proof wrapper, which derived data changes? Why must the surrounding verifier check the path against a trusted root?

## Limits and traceability

 The source map still labels this counterpart partial and parity not_verified. The described fixtures establish their particular observations, not full API equivalence or a working full node.

Pinned Java SHA-256: `3301b3722d2b1875f9682c68d6df13da154af8f3608ad90adc38b532f5cda7ac`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).

## Transaction-object Streamable integration

CoinProof now implements Streamable using its existing in-place reader. Its writer delegates first to Coin and then MMRProof on the same output buffer. A failed nested coin write leaves the coin prefix; a failed proof hash leaves the complete coin and preceding proof fields. The wrapper does not buffer each child before appending it.

The [transaction-object interface lab](../../../../../new-tests/streamable-transaction-objects.md) explains the source-order tests and existing generic callers. Both complete builds pass. The expanded interface suite passes 2,771 comparisons and 20,914 assertions per build across 20 types, with matching transcripts and empty native stderr. All seven affected regressions also pass in both builds. Full nullable/reference/API and node parity remain unverified.
