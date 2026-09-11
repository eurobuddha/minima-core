# txncreate — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/txn/txncreate.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txncreate.java) | [src/org/minima/system/commands/txn/txncreate.cpp](../../../../../../../../src/org/minima/system/commands/txn/txncreate.cpp) |

## What this file does

Create one named draft transaction with an empty transaction and witness.

## Follow the logic

1. Obtain the custom transaction database before parsing the required id.
2. Look up id and reject an existing row. The command enforces uniqueness here even though TxnDB.createTransaction itself simply appends.
3. Create the row, build the common reply and convert the newly looked-up row to JSON.

## How the translation is approached

C++ obtains the actual shared TxnDB through its runtime lookup and uses existing TxnRow, Transaction and Witness types. The lookup is a runtime dependency boundary; it does not supply full Main startup. It preserves the preflight duplicate check and creation-before-response order.

## What the tests tell us

Cases create new and duplicate IDs, supply missing/blank/non-string IDs, then list or export the database. Multi-command examples stop after a duplicate and reveal whether later commands execute.

The complete [driver](../../../../../../../../tools/transaction_commands_differential.py), [Java oracle](../../../../../../../../support/TransactionCommandsOracle.java) and [native harness](../../../../../../../../support/transaction_commands.cpp) run sequences against a real TxnDB bound to controlled runtime receivers. After every operation they compare its result, the entire ordered database wire representation and each transaction’s checked-monotonic flag. This detects changes surviving a failed command. Generic exceptions become ERROR; CommandException messages are preserved. The existing [normal report](../../../../../../../../validation/multisig-work-in-progress/normal/transaction-commands-differential-results.json) records 1,129 programs, 6,904 operations and zero failures. This guide did not rerun the suite; these totals cover all nine commands together.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxnDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [txncreate](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txncreate.java#L14), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txncreate.java#L19), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txncreate.java#L35), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txncreate.java#L40), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txncreate.java#L60).

## Check your understanding

Why should the lesson distinguish the command’s duplicate check from the lower-level database method?

## Limits and traceability

Full API, exact generic exceptions and node integration parity remain open. These offline draft-transaction tests do not mine, sign or broadcast a transaction. 

Pinned Java SHA-256: `cd975800aedf2309f7a65422c8cae9fbdca644b0f420f8676eb3367b8f0d5ff3`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
