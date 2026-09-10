# TxPoWGenerator — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/brains/TxPoWGenerator.java](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java) | [src/org/minima/system/brains/TxPoWGenerator.cpp](../../../../../../../src/org/minima/system/brains/TxPoWGenerator.cpp) |

## What this file does

Builds a candidate TxPoW from the current chain, user preferences, supplied transaction and a selected mempool subset.

## Follow the logic

1. Set height to tip plus one. Use a median-time window twice the normal size to bound the current clock: if too old choose median plus 50 seconds; if too far ahead choose median plus MAX_TIME_FUTURE.
2. Attach supplied main/burn data, advance current Magic values and set desired values from UserDB. Copy superparents, then replace all levels up through the tip’s superlevel with the tip ID.
3. Compute the block target and a transaction target from MAX_HASH divided by user hash rate. Smaller hash targets mean harder work. Clamp against the block target, then, if above Magic’s permitted target, divide that limit by 1.1 using DECIMAL64 rounding before integer conversion.
4. Stable-sort unused mempool entries by descending burn and retain the first 500. Remove the rest from RAM. Set MEMPOOL_FULL and retain the first removed burn as the threshold; on later non-full runs the boolean resets but the old MIN_MEMPOOL_BURN value is not cleared.
5. Reserve coin-proof IDs already used by the candidate’s main/burn witness. For each mempool transaction, reject counts above three, detect coin collisions and check size/work limits. A collision increments rejection and immediately continues, leaving that item in the mempool for now.
6. Run checkTxPoWSimple against the parent MMR and candidate block. Success adds its ID and reserves its proof IDs; an ordinary false checker result increments rejection but leaves valid true, so it is not immediately removed. Invalid limits or caught exceptions set valid false and trigger removal.
7. Stop after reaching Magic’s transaction count, then calculate transaction IDs and construct a TxBlock/tree node to obtain the resulting MMR root and total. Recalculate the candidate ID and return it.
8. Difficulty retargeting uses median-time blocks and average targets, bounds the speed ratio to 0.5–2.0 and caps it at the minimum-work target. Early heights use minimum work; identical median heights or nonpositive elapsed time retain the latest target. Median selection takes size/2 after stable sorting, so an even window chooses the upper middle.
9. precomputeTransactionCoinID leaves an inputless transaction alone. Otherwise outputs derive IDs from the first input and output index; an ELTOO first input instead resets outputs to the placeholder ID.

## How the translation is approached

C++ reuses ConsensusServices, exact decimal/BigInt operations, TxBlock/MMR construction and checker implementations. The explicit generateTxPoWAt clock adapter supports replay, and stable_sort preserves tie behavior. The shared-owner overload retains the supplied main and burn transaction/witness owners through TxPoW and TxBody. The older value convenience overload allocates owners and delegates. Selected TxPoW containers still have value-based call paths; the entire Java ownership graph is not certified.

## What the tests tell us

The [consensus oracle](../../../../../../../support/ConsensusOracle.java), [native harness](../../../../../../../support/consensus.cpp) and [driver](../../../../../../../tools/consensus_differential.py) compare generated wire, fullness/threshold, rejection/cache flags and removed IDs. Cases include own-input collisions, repeated rejects, mempool sizes 499/500/501/510, extreme hash rates/counts and both forced time clamps. Chain cases vary timestamp direction, median window, retarget bounds and superparents. PRNG bytes are reset after generation and timestamps deliberately force deterministic clamp branches. The inspected report has 2,890 passing total comparisons across generator/checker/relay work, not generator-only assertions.

The [transaction-check lab](../../../../../../../student-guide/new-tests/transaction-check.md) exercises the actual generator through MinimaDB service binding. It retains the generated TxPoW, mutates the stored transaction/witness, and deletes the row while inspecting owner identity and bytes. The stale validation-cache case demonstrates why copying supplied transactions would be incorrect.

The [coin-funding lab](../../../../../../new-tests/coin-selection-and-funding.md)
found omitted mempool work/size rejection logs while exercising send. The native
branches now use the existing MinimaLogger with the source text and ordering.
The source's capacity-removal and caught-exception logging paths still need a
separate parity review; this change does not claim the entire logger surface.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxPoWDB](../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java), [TxPoWTreeNode](../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [UserDB](../../../../../../../../upstream-study/src/org/minima/database/userprefs/UserDB.java), [Coin](../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [CoinProof](../../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java), [Magic](../../../../../../../../upstream-study/src/org/minima/objects/Magic.java), [Transaction](../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [TxBlock](../../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java), [TxPoW](../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [Witness](../../../../../../../../upstream-study/src/org/minima/objects/Witness.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MMRData](../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRData.java), [GlobalParams](../../../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java), [Crypto](../../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java).

Method navigation (declarations with bodies found in this file): [isMempoolFull](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L44), [getMinMempoolBurn](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L48), [calculateDifficultyData](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L55), [generateTxPoW](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L62), [generateTxPoW](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L66), [compare](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L161), [getBlockDifficulty](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L351), [getChainSpeed](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L419), [getAverageDifficulty](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L439), [getMedianTimeBlock](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L466), [getMedianTimeBlock](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L470), [compare](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L492), [precomputeTransactionCoinID](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L504), [main](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L548), [compare](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java#L557).

## Check your understanding

Two candidates fail differently: one collides with an already chosen coin, another fails the simple checker. Follow rejection count, valid and removal for each. At what later count can removal occur?

## Limits and traceability

Native logs omit several source messages, and generic ERROR normalization cannot establish exact error/stack behavior. Real-time unclamped generation, arbitrary service failures, the full reference graph and Main startup binding remain open. The counterpart is partial and parity not_verified.

Pinned Java SHA-256: `5a33742e0aac15e21f04d782dc8a179fc262cf8fca004b1b62bd74020da55a2e`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
