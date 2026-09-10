# txnbasics — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnbasics.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/txn/txnbasics.cpp)

## Purpose

txnbasics fills in discoverable proof/script data for a stored custom transaction, then refreshes its IDs.

## Follow the command

1. Require id, find the TxnDB row and throw Transaction not found when absent.
2. Pass the row’s live Transaction and Witness to txnutils.setMMRandScripts with exit-on-failure false. That mode skips unresolved coins and missing wallet scripts, while still allowing structural failures such as a missing history node to escape.
3. Precompute output coin IDs, calculate the transaction ID and return the row JSON. Existing signatures are not re-created here.

## Translation and dependencies

The native counterpart now compiles. It reuses txnutils’ permissive mode, TxPoWGenerator and Transaction ID serialization. Filling a witness does not require replacing the stored transaction object.

## Understanding the tests

The Java/C++ experiment combines resolved/unresolved ordinary and floating inputs, existing scripts and missing history. Assert actual appended proofs/scripts and refreshed IDs, plus partial witness mutation on later failure. Verify that permissive mode is not incorrectly treated as suppressing all exceptions.

- [Permissive proof/script helper](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnutils.java)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

The [transaction-input lab](../../../../../../../new-tests/transaction-inputs.md) explains the executed comparisons and their limits. Both builds pass the 671-program comparison and all 13 selected regressions; consult the lab for the scope and limitations.

## Limits

The command can succeed while leaving missing witness data. It is preparation for later validation, not proof that the transaction is spendable.

## Check your understanding

Why can a successful txnbasics result still fail txncheck?

Pinned Java SHA-256: `c3932c42a18b421eae622cb5652335b6ca23fd324ebe3fbfcabb16038c607fdc`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
