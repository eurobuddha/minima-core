# txnmine — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnmine.java) · [Native command](../../../../../../../../src/org/minima/system/commands/txn/txnmine.cpp)

## Purpose

txnmine builds and mines a TxPoW from a stored or serialized custom transaction, explicitly requesting that the miner not post it.

## Follow the command

1. Give id precedence over data. Look up a stored row or decode data as TxnRow; the data branch does not explicitly reject a null decoded row before dereferencing it.
2. Clear cached monotonic checks, precompute coin IDs and recalculate transaction ID on the selected row. An id-backed call changes the stored row even if mining later fails.
3. Generate TxPoW with its existing witness and calculate TxPoW ID. Call MineMaxTxPoW(false, txpow, 120000, false), where the final false controls posting.
4. On false mining result throw the timeout message; otherwise serialize the mined TxPoW and return txpowid plus data hex.

## Translation and dependencies

The native body reuses TxnRow decoding, the actual shared-owner generator and MineMaxTxPoW with posting disabled. It retains id/data precedence and does not store an imported row. The existing getMiner adapter still requires native Main startup binding; an unbound or null receiver throws rather than simulating successful mining.

## Understanding the tests

The [mining/posting lab](../../../../../../../../student-guide/new-tests/transaction-mining.md) explains the new source-derived corpus and records validation results. It exercises actual queued and synchronous mining, retained message/TxPoW owners, imported-row isolation, invalid data, missing receivers, failure after posting, and late deletion parsing. Generated PRNG bytes are fixed at miner entry for reproducible hashes; difficulty and transaction contents are preserved.

- [Cases](../../../../../../../../tools/transaction_mining_corpus.py)
- [Comparison driver](../../../../../../../../tools/wallet_commands_differential.py)
- [Native miner fixture](../../../../../../../../support/WalletMiningFixture.hpp)

## Limits

Actual mining is exercised in a controlled fixture. Returned data is TxPoW serialization, whereas input data is TxnRow serialization. The command does not add scripts/proofs or signatures automatically.

## Check your understanding

Why cannot the returned data be passed directly back as txnmine data without changing its decoder?

Pinned Java SHA-256: `690ca05e1e84c58e4ec0ab5dd5781d8e2fcaedae34e748f145c24cbdbe65393e`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
