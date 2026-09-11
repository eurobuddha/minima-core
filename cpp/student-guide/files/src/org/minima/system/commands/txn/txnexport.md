# txnexport — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/txn/txnexport.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnexport.java) | [src/org/minima/system/commands/txn/txnexport.cpp](../../../../../../../../src/org/minima/system/commands/txn/txnexport.cpp) |

## What this file does

Serialize a stored draft transaction to HEX or a file.

## Follow the logic

1. Read id and showtxn and require the row.
2. If file is present, resolve it, attempt to delete an existing destination, then serialize the row to the file. Return absolute path and formatted size.
3. Otherwise serialize the row to bytes first, optionally call toJSON for the displayed transaction, and return the previously captured data bytes.

## How the translation is approached

C++ obtains the actual shared TxnDB through its runtime lookup and uses existing TxnRow, Transaction and Witness types. The lookup is a runtime dependency boundary; it does not supply full Main startup. It reuses MiniFile and MiniFormat. The ordering of serialization before optional JSON is retained because row JSON can precompute coin IDs.

## What the tests tell us

The driver exports Java-generated rows to HEX and files, reads files back with txnview/import and toggles showtxn. It compares command responses and the database state after each operation.

The complete [driver](../../../../../../../../tools/transaction_commands_differential.py), [Java oracle](../../../../../../../../support/TransactionCommandsOracle.java) and [native harness](../../../../../../../../support/transaction_commands.cpp) run sequences against a real TxnDB bound to controlled runtime receivers. After every operation they compare its result, the entire ordered database wire representation and each transaction’s checked-monotonic flag. This detects changes surviving a failed command. Generic exceptions become ERROR; CommandException messages are preserved. The existing [normal report](../../../../../../../../validation/multisig-work-in-progress/normal/transaction-commands-differential-results.json) records 1,129 programs, 6,904 operations and zero failures. This guide did not rerun the suite; these totals cover all nine commands together.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxnDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java), [TxnRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [MiniFormat](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFormat.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [txnexport](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnexport.java#L19), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnexport.java#L24), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnexport.java#L45), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnexport.java#L50), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnexport.java#L102).

## Check your understanding

Why must an implementation avoid silently moving row.toJSON before the byte serialization?

## Limits and traceability

Full API, exact generic exceptions and node integration parity remain open. These offline draft-transaction tests do not mine, sign or broadcast a transaction. File permission, interrupted writes and every filesystem failure are not covered by these round trips.

Pinned Java SHA-256: `e5904bd04bc0a4dc7b4d452a43e4819a2abd977c3ed339c8dee168fe7eac0d9c`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
