# txnscript — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnscript.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/txn/txnscript.cpp)

## Purpose

txnscript adds wallet-known scripts automatically or explicit script/MMR-proof pairs to a stored witness.

## Follow the command

1. Read id, scripts JSONObject (default empty) and auto before looking up the row. Thus malformed scripts can fail even when auto:true would ignore its contents.
2. In auto mode, inspect each input address. If its witness script is missing, fetch a wallet ScriptRow and add a plain ScriptProof when found; absent wallet scripts are silently skipped.
3. In manual mode, iterate script keys and cast each value to String. Empty proof text creates a plain ScriptProof; otherwise decode MMRProof bytes and combine them with that script.
4. Add each proof immediately, then return the current row JSON. The local added-script text list is not returned. No transaction ID recalculation or script execution is performed.

## Translation and dependencies

The command reuses the actual runtime TxnDB, MinimaDB Wallet, ScriptProof/MMRProof constructors and Witness insertion. ScriptProof hashes the supplied script text; it does not call Contract.cleanScript here. Native JSONObject entries retain the pinned Java LinkedHashMap insertion order. Auto/manual exclusivity and failure order are preserved.

## Understanding the tests

The new corpus checks plain and branched proofs, malformed/truncated bytes, null and nonstring values, raw whitespace and Unicode scripts, duplicate insertion, auto-mode skipping and checked/runtime wallet failures. Each operation records SQL calls, logs and complete transaction database bytes. Thus a valid first script followed by a failing second entry must leave the first witness addition visible. Explicit guards check successful insertion and duplicate suppression.

The comparison reuses [WalletCommandsOracle.java](../../../../../../../../support/WalletCommandsOracle.java), the [native harness](../../../../../../../../support/wallet_commands.cpp) and [source-order corpus](../../../../../../../../tools/witness_commands_corpus.py). Only SQL is a recording driver; chain, transaction, proof and witness objects are actual implementations. Like the earlier custom-transaction tests, the fixture attaches a real TxnDB through Java field setup and the existing native runtime lookup. Merely creating MinimaDB does not initialize that database in either implementation; this fixture does not exercise full startup.

The 418-program witness comparison passes in both builds: 1,128 operations and 8,637 SQL calls. Both full builds and all nine selected regressions pass: normal in 44.49 seconds and ASAN/UBSAN in 182.20 seconds. See the [witness-command lab](../../../../../../../new-tests/witness-commands.md) for the failures that led to corrections and the final verification status.

## Limits

Adding a proof does not establish that it resolves the intended script address or that executing the script returns true. Those need witness/VM checks.

## Check your understanding

Why can auto:true with an invalid scripts parameter fail before looking at any wallet scripts?

Pinned Java SHA-256: `d6a9ca37c93b2c31436a0e92bf8c994bcf6ffab854c9daad822fa34130712bce`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
