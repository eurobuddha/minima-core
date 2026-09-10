# TxBlock — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/objects/TxBlock.java](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java) | [src/org/minima/objects/TxBlock.cpp](../../../../../../src/org/minima/objects/TxBlock.cpp) |

## What this file does

A synchronization/archive block containing its main TxPoW, prior MMR peaks, input coin proofs and newly created coins. It derives the coin changes in the exact order required by the block.

## Follow the logic

1. Keep the main TxPoW reference and previous peaks. Build a child MMR over the parent so input proofs can be reconstructed without replacing the parent layer.
2. Process the main TxPoW first, then each ID in its block-transaction list. Search the supplied TxPoW collection for the matching ID, ignoring collection order; missing required transactions throw. For each TxPoW process the normal transaction before the burn transaction.
3. Skip an empty transaction. For each witness coin proof, update the working MMR with its unspent leaf data and obtain a fresh proof to the current peak. Record that coin/proof pair in the spent list. This method prepares proof data; it does not independently validate the old proof first.
4. Generate output coins only when at least one witness coin proof exists. For the genesis child MMR at time one, use the transaction ID as the base; otherwise use the first witness coin’s ID. Derive each output coin ID from that base and output index.
5. Copy each output with its calculated ID. If storeState is true, attach the transaction’s state. Otherwise retain the state only in the removed-state table so notifications can still include it.
6. For TOKENID_CREATE outputs, build token metadata using the actual output coin ID, creator scale/name/script, output amount and main block number. Replace the placeholder token ID and attach the new Token.
7. Serialize main TxPoW, then count/entries for prior peaks, spent proofs and new coins. Removed-state notification data is deliberately absent from the wire.
8. Reading clears the three lists before decoding the main TxPoW, then appends each category. It does not clear the removed-state table. A failed read can leave the old TxPoW with cleared lists or a new TxPoW with partially filled lists, depending on where failure occurs.

## How the translation is approached

The native runtime overload accepts shared_ptr<TxPoW> and shared transaction lists to preserve main-object identity. A separate value overload remains for native value callers and copies those values. Previous peak entries are shared pointers, but coin/proof/state collections still use native value containers, so not every Java collection alias is reproduced. The mutating read method preserves the observed clearing/replacement order; the converter catches only wire::IOException and can inject logging.

## What the tests tell us

The ordinary object fixtures vary parent size, flags, block time, normal/burn inputs, token creation and state retention; compare the constructed block’s bytes and derived coin information; and reorder supplied transactions to check that the block’s ID list controls processing. More specifically, [TxBlockOwnershipOracle](../../../../../../support/TxBlockOwnershipOracle.java) and [native ownership harness](../../../../../../support/txblock_ownership.cpp) retain references to the old/main TxPoW, mutate through each reference, construct a tree node, and compare identity labels, nonce values, wire output, list sizes and removed-state data. The [ownership driver](../../../../../../tools/txblock_ownership_differential.py) reads every truncated prefix of its reference block, probes null/shared constructor arguments and missing transactions, and checks consumed offsets and partial receiver state. The inspected report records 1,558 matching programs and 6,219 operations, including shared-checker scenarios beyond TxBlock. Collection aliases and full runtime are explicitly incomplete; null-pointer expression text is normalized.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MMR](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMR.java), [MMREntry](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntry.java), [MMREntryNumber](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMREntryNumber.java), [MMRProof](../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java), [MinimaLogger](../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [TxBlock](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L47), [TxBlock](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L50), [TxBlock](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L55), [getTxpoWFromList](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L86), [calculateCoins](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L99), [calculateCoins](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L107), [getTxPoW](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L204), [getPreviousPeaks](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L208), [getInputCoinProofs](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L212), [getOutputCoins](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L216), [removedState](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L220), [writeDataStream](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L229), [readDataStream](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L249), [ReadFromStream](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L272), [convertMiniDataVersion](../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java#L281).

A missing token-creation payload now throws the existing Java-style null exception at the creator access. The transaction-check corpus caught the former bad_optional_access mismatch during generated block construction. The source and native construction algorithms are otherwise reused. See the [transaction-check lab](../../../../../../student-guide/new-tests/transaction-check.md) for the experiment and results.

## Check your understanding

Why does reversing the supplied transaction collection not reverse output-coin order? Then predict which references still point to the old TxPoW after a successful read into the same TxBlock.

## Limits and traceability

Source-map status remains partial/not_verified. Matching main TxPoW identity and prefix-failure state does not establish all nested Coin, CoinProof, peak-list or removed-state list aliases. Final MMR construction and consensus acceptance occur in separate components, and a serializable TxBlock is not automatically a valid block.

Pinned Java SHA-256: `5e051899b90f773b5f92f9e8942b1a1b322247f9e018f9073142980e97696284`. Mapping: [source-map.json](../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../REVIEW.md).
