# txnpost — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnpost.java) · [Native command](../../../../../../../../src/org/minima/system/commands/txn/txnpost.cpp)

## Purpose

txnpost builds a TxPoW from a stored transaction, optionally adds witnesses and a linked burn, and queues or performs mining.

## Follow the command

1. Read id, burn, auto and mine. The active default for auto is false despite the constructor/help implying automatic setup. Call postTxn before reading txndelete.
2. postTxn rejects negative burn, requires the row, clears monotonic-check cache and optionally fills scripts/proofs in strict mode. Precompute coin IDs and recalculate transaction ID.
3. For positive burn, collect excluded coin IDs from witness CoinProofs and call txnutils.createBurnTransaction linked to the main transaction ID. Generate TxPoW with both transactions; zero burn uses only the main one.
4. Calculate TxPoW ID and require a tip. Compare serialized size excluding block transaction IDs with tip Magic’s maximum, rejecting only strictly greater size.
5. Mine synchronously for up to 120 seconds or queue asynchronous mining. Once this returns, parse txndelete and optionally delete the stored row, then format TxPoW JSON.

## Translation and dependencies

The native body reuses Command parsing, actual MinimaDB custom rows, strict txnutils witness setup, linked burn construction, the shared-owner generator and actual miner. postTxn returns a shared TxPoW so queued messages and stored transaction/witness owners remain connected. The existing getMiner adapter still requires Main startup binding. Size checks and deletion timing follow the source, including work already performed before a late failure.

## Understanding the tests

The [mining/posting lab](../../../../../../../../student-guide/new-tests/transaction-mining.md) explains the new source-derived corpus and records validation results. It exercises actual queued and synchronous mining, retained message/TxPoW owners, imported-row isolation, invalid data, missing receivers, failure after posting, and late deletion parsing. Generated PRNG bytes are fixed at miner entry for reproducible hashes; difficulty and transaction contents are preserved.

- [Cases](../../../../../../../../tools/transaction_mining_corpus.py)
- [Comparison driver](../../../../../../../../tools/wallet_commands_differential.py)
- [Native miner fixture](../../../../../../../../support/WalletMiningFixture.hpp)

## Limits

An asynchronous return is not confirmation, and txndelete removes a local draft without undoing a queued TxPoW. The fixture records local Main messages and does not run a network node.

## Check your understanding

Why can a command fail after mining has already been requested?

Pinned Java SHA-256: `3291c1d650a9b4ab3c3922e8204cba35655b25064ac4b66747e5d7654bc36386`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
