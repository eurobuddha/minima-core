# txnutils — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnutils.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/txn/txnutils.cpp)

## Purpose

txnutils supplies shared proof/script construction and linked burn-transaction creation. It is a helper class, not a registered Command subclass.

## Follow the command

1. The transaction proof helper resolves each ELTOO input using getFloatingCoin and ordinary input using searchCoin. It gathers resolved coins for witness data without replacing the original transaction inputs. Strict mode throws on missing coins; permissive mode skips them.
2. Choose a common historical MMR node from the newest resolved creation height, clamped to proof history. Missing history throws even in permissive mode. Append each CoinProof, then add a wallet script only if the witness lacks that address; strict missing-script failure occurs after proof insertion.
3. The single-coin overload uses the passed coin directly, does not resolve its ID and always attempts wallet script insertion. It has no existing-script check and throws CommandException for a missing script after adding the proof.
4. createBurnTransaction creates an unstored temp row. Fetch relevant simple Minima coins and skip supplied excluded IDs, mining coins and mempool coins; accumulate until the burn amount is covered. There is no explicit confirmation-age filter in this helper.
5. Add inputs, choose the clamped proof block and append proofs/scripts, collecting unique signing keys. Validate the Minima amount only after this preparation. Add only positive change as a Minima output with storeState false: the missing input value is the burn.
6. Set link hash to the supplied main transaction ID, precompute coin IDs and calculate the burn transaction ID. Optionally sign once per required public key; default overload signs. Return the temp row without storing or posting it.

## Translation and dependencies

The native counterpart now compiles. It reuses searcher, MMR, Witness, wallet script/signing and transaction serialization. A burn is represented by input/output difference plus a link, not by an output sent to a special address.

The existing burn builder now also serves txnauto's static helper through
AutomaticTransaction.hpp. The burn path retains its late amount check and
link assignment; txnauto additionally validates before coin search and leaves
the default link untouched. See the [automatic-transaction lab](../../../../../../../new-tests/automatic-transaction.md).

## Understanding the tests

The helper experiments distinguish strict/permissive failures, original input versus resolved proof coin, floating selection, duplicated calls and proof-before-script errors. Burn cases cover excluded/pending coins, zero/invalid amounts, change, exact link hash, deduplicated signatures and sign:false. Assert key-use consumption on late signing failure.

- [Permissive caller](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnbasics.java)
- [Strict caller and linked burn](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnpost.java)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

The [transaction-input lab](../../../../../../../new-tests/transaction-inputs.md) explains the executed comparisons and their limits. Both builds pass the 671-program comparison and all 13 selected regressions; consult the lab for the scope and limitations.

## Limits

Native Main must still bind the explicit miner lookup used by burn construction. The SQL receiver used in these tests records calls and does not provide durable storage.

These helpers can mutate a live witness before throwing and do not enforce every transaction validity rule. Callers such as txnbasics and txnpost intentionally choose different strictness.

## Check your understanding

Why must a burn transaction exclude the main transaction’s already-used coins?

Pinned Java SHA-256: `a16f30cf7a00b5f2a9dd4440e2dd8d095f613b1da17ecd92fd0e4c099e3c5a7d`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
