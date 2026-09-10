# txnimport — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/txn/txnimport.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnimport.java) | [src/org/minima/system/commands/txn/txnimport.cpp](../../../../../../../../src/org/minima/system/commands/txn/txnimport.cpp) |

## What this file does

Decode a serialized row, optionally rename it, and insert it into the draft database.

## Follow the logic

1. Obtain the database. If file is present, choose it ahead of data, require its existence and read its bytes. Otherwise require data and decode it through Command.
2. Convert bytes to TxnRow. If id is present, apply it to the decoded row.
3. Call addCompleteTransaction, which removes matching IDs and appends the imported row.
4. Return its JSON; failures during response construction can occur after insertion.

## How the translation is approached

C++ obtains the actual shared TxnDB through its runtime lookup and uses existing TxnRow, Transaction and Witness types. The lookup is a runtime dependency boundary; it does not supply full Main startup. It preserves file precedence and uses the shared row decoder. Missing decoded rows currently become generic native errors, whose exact class is not compared in this suite.

## What the tests tell us

The driver imports valid, appended, truncated and malformed Java-produced row bytes, tests explicit renaming and duplicate IDs, and provides file and data together. Follow-up export/list reveals changes already made before an error.

The complete [driver](../../../../../../../../tools/transaction_commands_differential.py), [Java oracle](../../../../../../../../support/TransactionCommandsOracle.java) and [native harness](../../../../../../../../support/transaction_commands.cpp) run sequences against a real TxnDB bound to controlled runtime receivers. After every operation they compare its result, the entire ordered database wire representation and each transaction’s checked-monotonic flag. This detects changes surviving a failed command. Generic exceptions become ERROR; CommandException messages are preserved. The existing [normal report](../../../../../../../../build/transaction-commands-differential-results.json) records 1,129 programs, 6,904 operations and zero failures. This guide did not rerun the suite; these totals cover all nine commands together.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxnDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java), [TxnRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [txnimport](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnimport.java#L18), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnimport.java#L23), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnimport.java#L49), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnimport.java#L54), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnimport.java#L108).

## Check your understanding

Why should a failed import response not automatically imply the database is unchanged?

## Limits and traceability

Full API, exact generic exceptions and node integration parity remain open. These offline draft-transaction tests do not mine, sign or broadcast a transaction. 

Pinned Java SHA-256: `de026d8434227c34ba8a4819d3bb50d80d18ba94998441841d16125e279e4513`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

### Multisig integration follow-up

Malformed imports now use the existing JavaNullPointerException representation instead of a generic native runtime error. An explicit replacement ID is parsed before a null decoded row is dereferenced, matching Java argument evaluation. The multisig workflow comparison covers the resulting nested error and retained-row behavior.
