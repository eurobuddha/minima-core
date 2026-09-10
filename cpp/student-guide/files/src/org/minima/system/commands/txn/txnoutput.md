# txnoutput — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/txn/txnoutput.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnoutput.java) | [src/org/minima/system/commands/txn/txnoutput.cpp](../../../../../../../../src/org/minima/system/commands/txn/txnoutput.cpp) |

## What this file does

Append a coin output, using token metadata to convert displayed amounts into stored amounts.

## Follow the logic

1. Read id, amount, address and storestate. Default the token ID to Minima; when another token ID is supplied, look up its metadata and reject an unknown token.
2. For an attached token, convert the requested token amount through getScaledMinimaAmount; otherwise retain the amount.
3. Construct the output coin with the output placeholder ID and optional Token metadata before looking up the draft row.
4. Append the coin, precompute transaction output IDs, recalculate the transaction ID and return row JSON.

## How the translation is approached

C++ obtains the actual shared TxnDB through its runtime lookup and uses existing TxnRow, Transaction and Witness types. The lookup is a runtime dependency boundary; it does not supply full Main startup. The implementation reuses Coin, Token and TxPoWGenerator, so it does not duplicate ID derivation or scaling.

## What the tests tell us

Cases cross negative/zero/fractional/extreme amounts with valid and malformed addresses. Java-generated imported tokens exercise scaling and keep-state flags. Unknown token IDs, missing rows and follow-up exports expose error ordering and mutations.

The complete [driver](../../../../../../../../tools/transaction_commands_differential.py), [Java oracle](../../../../../../../../support/TransactionCommandsOracle.java) and [native harness](../../../../../../../../support/transaction_commands.cpp) run sequences against a real TxnDB bound to controlled runtime receivers. After every operation they compare its result, the entire ordered database wire representation and each transaction’s checked-monotonic flag. This detects changes surviving a failed command. Generic exceptions become ERROR; CommandException messages are preserved. The existing [normal report](../../../../../../../../build/transaction-commands-differential-results.json) records 1,129 programs, 6,904 operations and zero failures. This guide did not rerun the suite; these totals cover all nine commands together.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxnDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java), [TxnRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [Coin](../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [Token](../../../../../../../../../upstream-study/src/org/minima/objects/Token.java), [Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [TxPoWGenerator](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java), [TxPoWSearcher](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [txnoutput](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnoutput.java#L22), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnoutput.java#L27), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnoutput.java#L64), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnoutput.java#L69), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnoutput.java#L128).

## Check your understanding

If both the token ID and transaction ID are missing, which error should appear first? Follow the actual lookup order.

## Limits and traceability

Full API, exact generic exceptions and node integration parity remain open. These offline draft-transaction tests do not mine, sign or broadcast a transaction. 

Pinned Java SHA-256: `5985f7558540e7fa83ac486957f59bb000a94ead479f285d1477a6103f4657e6`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
