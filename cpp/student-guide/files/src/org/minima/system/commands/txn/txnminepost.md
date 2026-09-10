# txnminepost — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnminepost.java) · [Native command](../../../../../../../../src/org/minima/system/commands/txn/txnminepost.cpp)

## Purpose

txnminepost decodes a pre-mined TxPoW and hands it to Main’s mined-transaction event path.

## Follow the command

1. Parse required data with Command.getDataParam and decode it with TxPoW.convertMiniDataVersion.
2. Post MAIN_TXPOWMINED with a txpow object before constructing the JSON response.
3. Return the TxPoW JSON under response.data. There is no explicit proof-of-work, signature or null check in this wrapper; downstream processing is responsible for acceptance.

## Translation and dependencies

The native body reuses TxPoW decoding and Message ownership. A Main-message receiver follows the existing command callback pattern, with an unbound default that throws for missing Main. It forwards even a null decoded object before trying to create response.data JSON. Native startup must bind the receiver to the actual Main message processor.

## Understanding the tests

The [mining/posting lab](../../../../../../../../student-guide/new-tests/transaction-mining.md) explains the new source-derived corpus and records validation results. It exercises actual queued and synchronous mining, retained message/TxPoW owners, imported-row isolation, invalid data, missing receivers, failure after posting, and late deletion parsing. Generated PRNG bytes are fixed at miner entry for reproducible hashes; difficulty and transaction contents are preserved.

- [Cases](../../../../../../../../tools/transaction_mining_corpus.py)
- [Comparison driver](../../../../../../../../tools/wallet_commands_differential.py)
- [Native miner fixture](../../../../../../../../support/WalletMiningFixture.hpp)

## Limits

Posting an event is not chain acceptance or confirmation. The fixture records locally posted events; downstream full-node acceptance remains unverified.

## Check your understanding

Why must failure tests inspect the message queue even when response formatting throws?

Pinned Java SHA-256: `f1625966bdb7414cf8d356d235c7ccef47ddb5b975bb681d060e988fc69d7424`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
