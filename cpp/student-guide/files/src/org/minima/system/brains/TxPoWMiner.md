# TxPoWMiner — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/brains/TxPoWMiner.java](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java) | [src/org/minima/system/brains/TxPoWMiner.cpp](../../../../../../../src/org/minima/system/brains/TxPoWMiner.cpp) |

## What this file does

Searches for a header hash below the transaction target, tracks coins reserved by mining and coordinates asynchronous mining messages.

## Follow the logic

1. Before queued or synchronous work, add unique input coin IDs from both main and burn transactions to a reservation list. This is a set-like list, not a per-job reference count: overlapping jobs can share one entry.
2. Set the body hash, then use a deliberately large decimal nonce so its serialized field has room. Serialize the header once; each iteration copies the next BigInteger byte sequence beginning at byte offset four and hashes the full modified header with SHA3.
3. Success requires hash < target, strictly. Decode the patched header to recover its resulting MiniNumber nonce, then set that nonce on the TxPoW and recalculate ID/size. The byte counter itself is not directly assigned as the final decimal nonce.
4. Async processing posts a starting MAIN_MINING message before hashing. If the worker stops, the loop exits with final nonce zero but still performs the normal end/mined posts and reservation removal.
5. The source adds automine to the starting message again when constructing the ending message. Consequently the ending message lacks that field. Retained-message identity matters to a faithful port.
6. After async completion, automine schedules a pulse after the configured timer and a block-miner broadcast halfway through that delay. Pulse with no tip schedules a 30-second retry; TXBLOCK_NODE suppresses those generation branches.
7. Synchronous mining ignores worker-running state and checks elapsed time only after 10,001 failed hashes. Timeout uses strict greater-than and returns immediately, retaining reservations and the initial large nonce. Other exceptions before cleanup can also retain reservations.
8. Benchmark methods execute getAsInt(hashCount) iterations but use the original MiniNumber count as the speed numerator. Nonpositive measured elapsed time is replaced with one millisecond; a fractional or wrapped count therefore deserves a boundary test.

## How the translation is approached

C++ reuses MessageProcessor/TimerMessage, real SHA3, BigInt Java byte encoding and TxHeader decoding. A service object binds Main, timers, generator, network, logs and clock. Shared messages retain TxPoW identity; a native mutex protects reservation access, while the pinned Java list has no equivalent per-method lock.

## What the tests tell us

The [Java miner oracle](../../../../../../../support/TxPoWMinerOracle.java), [native harness](../../../../../../../support/txpow_miner.cpp) and [driver](../../../../../../../tools/txpow_miner_differential.py) execute real mining loops with easy targets and impossible-target timeouts. They compare final wire/IDs, queued/retained messages, duplicate reservations, post failures, mutation before processing, stopped loops and timers. The inspected [report](../../../../../../../build/txpow-miner-results.json) has 357 matching programs and 1,352 operations. Separately, [worker test source](../../../../../../../support/txpow_miner_worker.cpp) asserts sixteen real queued results, ownership through cancellation, exception recovery, a strict timeout boundary, benchmark arithmetic and routing callbacks. Reading that test source is not a fresh run; the differential report does not include its assertions.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [Coin](../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [Transaction](../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [TxHeader](../../../../../../../../upstream-study/src/org/minima/objects/TxHeader.java), [TxPoW](../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [Witness](../../../../../../../../upstream-study/src/org/minima/objects/Witness.java), [MiniData](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Main](../../../../../../../../upstream-study/src/org/minima/system/Main.java), [NIOManager](../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java), [NIOMessage](../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOMessage.java), [GeneralParams](../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [Crypto](../../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Message](../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java), [MessageProcessor](../../../../../../../../upstream-study/src/org/minima/utils/messages/MessageProcessor.java), [TimerMessage](../../../../../../../../upstream-study/src/org/minima/utils/messages/TimerMessage.java).

Method navigation (declarations with bodies found in this file): [TxPoWMiner](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java#L41), [mineTxPoWAsync](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java#L47), [processMessage](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java#L60), [addMiningCoins](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java#L213), [removeMiningCoins](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java#L237), [checkForMiningCoin](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java#L255), [MineMaxTxPoW](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java#L262), [MineMaxTxPoW](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java#L266), [calculateHashRateOld](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java#L372), [calculateHashSpeed](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java#L400), [main](../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java#L455).

## Check your understanding

Two jobs reserve the same coin. When the first finishes, why can the second still be active while checkForMiningCoin returns false? How would you preserve this source behavior without accidentally claiming it is a reference-counted reservation system?

## Limits and traceability

Full Main/network startup is incomplete. Java NPE expression text is normalized, arbitrary nonce-buffer exhaustion and concurrent Java races are not exhaustively matched, and the worker test supplies controlled routing receivers. Partial counterpart; parity not_verified.

Pinned Java SHA-256: `faf2b371e08eb2275726b9a405d7f66e7cea1b7536e5b5b5e85ad2da04585630`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).

## Mining-command integration

The native async and synchronous entry methods now permit virtual dispatch, as Java does. The command fixture fixes only the generated PRNG bytes at entry and delegates to the actual miner bodies. It compares wire bytes, posted and pending messages, retained owners and coin reservations after row deletion. False/throwing results are separate injected failure cases. See the [mining/posting lab](../../../../../../new-tests/transaction-mining.md).

## Runtime demonstration update

The added source main calls the old hash-rate helper for 10,000 hashes, again
for 1,000,000, then the header hash-speed helper for 1,000,000. Those full counts
remain in the native entry point. The source logging is commented out, so successful
execution produces no output. The test executes the real calls and records duration;
it does not instrument internal loop counts or claim speed equality between languages.

See the [runtime main lab](../../../../../../new-tests/runtime-main-methods.md) for the actual test evidence and limits.
