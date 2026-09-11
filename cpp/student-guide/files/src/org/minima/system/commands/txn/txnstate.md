# txnstate — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/txn/txnstate.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnstate.java) | [src/org/minima/system/commands/txn/txnstate.cpp](../../../../../../../../src/org/minima/system/commands/txn/txnstate.cpp) |

## What this file does

Set a numbered state value on a named draft transaction and recalculate its ID.

## Follow the logic

1. Read id, port and value strings, then find the row.
2. Parse the port with Integer.parseInt and construct StateVariable from port/value. This supplies its own type and port normalization.
3. Add the state variable to the transaction, calculateTransactionID, then return the row JSON.

## How the translation is approached

C++ obtains the actual shared TxnDB through its runtime lookup and uses existing TxnRow, Transaction and Witness types. The lookup is a runtime dependency boundary; it does not supply full Main startup. It reuses javaParseInt for Unicode decimal digits and signed32-bit bounds, rather than a permissive platform parser.

## What the tests tell us

The driver crosses boundary ports including -1,255,256,int extremes, fractions, Unicode digits and overflow with numeric/Boolean/HEX/string/malformed values. Each case then lists the row and compares database bytes.

The complete [driver](../../../../../../../../tools/transaction_commands_differential.py), [Java oracle](../../../../../../../../support/TransactionCommandsOracle.java) and [native harness](../../../../../../../../support/transaction_commands.cpp) run sequences against a real TxnDB bound to controlled runtime receivers. After every operation they compare its result, the entire ordered database wire representation and each transaction’s checked-monotonic flag. This detects changes surviving a failed command. Generic exceptions become ERROR; CommandException messages are preserved. The existing [normal report](../../../../../../../../validation/multisig-work-in-progress/normal/transaction-commands-differential-results.json) records 1,129 programs, 6,904 operations and zero failures. This guide did not rerun the suite; these totals cover all nine commands together.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxnDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java), [TxnRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [StateVariable](../../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java), [Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [txnstate](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnstate.java#L17), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnstate.java#L22), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnstate.java#L46), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnstate.java#L51), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnstate.java#L84).

## Check your understanding

Which validation belongs to Integer.parseInt and which belongs to StateVariable? Use port256 as an example.

## Limits and traceability

Full API, exact generic exceptions and node integration parity remain open. These offline draft-transaction tests do not mine, sign or broadcast a transaction. 

Pinned Java SHA-256: `2f6d6c07ac5da62d3bfaa20dac159f7ac1957d22c8add35e30c1fe167334b0ac`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
