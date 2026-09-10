# txnmmr — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnmmr.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/txn/txnmmr.cpp)

## Purpose

txnmmr appends proofs for inputs beyond the witness’s existing proof count, treating that count as an already-covered prefix.

## Follow the command

1. Require a stored transaction, count existing CoinProofs and obtain the current tip.
2. Choose proof block as the maximum of tip minus proof-history length and every input’s creation height. Look up that historical node without an explicit null guard.
3. Walk inputs with a one-based counter, skipping all positions at or below the original proof count. For later inputs, obtain the entry’s proof from the selected MMR and append a CoinProof.
4. Return the row JSON. Existing proof identities, ordering, block times and validity are not checked, and the command adds no scripts and does not explicitly recalculate the transaction ID. Its final TxnRow.toJSON call can precompute output coin IDs.

## Translation and dependencies

The command reuses the actual runtime TxnDB, MinimaDB chain owner, MMR.getProofToPeak and Witness.addCoinProof. It preserves positional skipping, including mismatched preexisting proofs. The original source does not validate the chosen proof or replace earlier proofs. Native CoinProof now stores a Coin handle sharing its field storage. The [coin ownership lab](../../../../../../../new-tests/coin-ownership.md) compares post-construction mutations through transaction and witness owners in 220 programs. MMRProof identity, state-list sharing across different coins and full runtime ownership remain separate incomplete areas.

## Understanding the tests

The new corpus supplies a twelve-node chain and coins from the existing Java SearcherOracle. Cases vary history length, creation height, missing historical nodes, entry numbers, zero/partial/excess witness proofs and repeated calls. It compares the complete custom transaction database bytes and monotonic flags after each operation. Success guards check exact resulting proof counts. Existing proofs are deliberately allowed to describe different coins: skipping depends only on their count.

The comparison reuses [WalletCommandsOracle.java](../../../../../../../../support/WalletCommandsOracle.java), the [native harness](../../../../../../../../support/wallet_commands.cpp) and [source-order corpus](../../../../../../../../tools/witness_commands_corpus.py). Only SQL is a recording driver; chain, transaction, proof and witness objects are actual implementations. Like the earlier custom-transaction tests, the fixture attaches a real TxnDB through Java field setup and the existing native runtime lookup. Merely creating MinimaDB does not initialize that database in either implementation; this fixture does not exercise full startup.

The 418-program witness comparison passes in both builds: 1,128 operations and 8,637 SQL calls. Both full builds and all nine selected regressions pass: normal in 44.49 seconds and ASAN/UBSAN in 182.20 seconds. See the [witness-command lab](../../../../../../../new-tests/witness-commands.md) for the failures that led to corrections and the final verification status.

## Limits

A complete proof count does not imply correct proofs. The selected node is common to newly added proofs only; existing proofs are left intact.

## Check your understanding

If the witness has one proof for input 3, which inputs will this command skip or append?

Pinned Java SHA-256: `280831b6864205fab4ce086a297be92ed7ebd2ec178291c449c360b53358a8c2`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
