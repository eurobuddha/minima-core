# TxnDB — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/userprefs/txndb/TxnDB.java](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java) | [src/org/minima/database/userprefs/txndb/TxnDB.cpp](../../../../../../../../src/org/minima/database/userprefs/txndb/TxnDB.cpp) |

## What this file does

Manage the ordered list of named draft transaction rows and persist its serialized form through UserDB.

## Follow the logic

1. createTransaction appends a fresh row without checking duplicate IDs. addCompleteTransaction instead deletes every matching ID, then appends the supplied row.
2. getTransactionRow returns the first matching row. deleteTransaction constructs and replaces the list object, retaining only nonmatching rows; held references to the old list remain distinct. clearTxns clears the current list in place.
3. saveDB serializes into UserDB. loadDB skips the zero sentinel; otherwise it decodes another database and replaces the current list with the decoded list.
4. Wire output writes a MiniNumber row count followed by rows. Reading immediately installs a fresh list, reads the count and appends rows one by one, so truncation can leave a partial list.
5. The conversion helper catches IOException, logs it and returns null; it does not catch arbitrary runtime exceptions.

## How the translation is approached

C++ uses a shared vector of shared TxnRow pointers to reproduce list replacement versus in-place clear. It retains the UserDB owner/resolver boundary and uses the existing bounded wire decoders. Conversion logging still uses a legacy clog message, so logger parity remains open.

## What the tests tell us

The driver generates30 Java row/database fixtures, adds trailing bytes and many truncations, and compares conversion versus in-place reads. Its125 seeded50-operation sequences retain list/row aliases, rename/reinsert rows, mutate held lists and save/load preference bytes. Observations include current IDs, held-list IDs and row contents.

The complete [TxnDB driver](../../../../../../../../tools/txndb_differential.py), [Java oracle](../../../../../../../../support/TxnDBOracle.java) and [native harness](../../../../../../../../support/txndb.cpp) compare source-derived serialized fixtures, partial byte consumption and persistent operation sequences. The existing [normal report](../../../../../../../../validation/streamable-txbody-work-in-progress/normal/txndb-differential-results.json) records 2,503 programs passed and zero failures. I/O failures and other exceptions are distinguished only as IO_ERROR versus ERROR, so fine-grained error text/classes and logger behavior are not established. No test was rerun for this guide.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [Witness](../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java).

Method navigation (declarations with bodies found in this file): [TxnDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java#L21), [loadDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java#L23), [saveDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java#L31), [createTransaction](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java#L35), [addCompleteTransaction](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java#L39), [getTransactionRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java#L47), [deleteTransaction](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java#L56), [listTxns](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java#L71), [clearTxns](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java#L75), [convertMiniDataVersion](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java#L82), [writeDataStream](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java#L103), [readDataStream](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java#L112), [ReadFromStream](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java#L120).

## Check your understanding

Hold listTxns(), then delete a row and then clearTxns(). Which list does each operation change?

## Limits and traceability

Full mutable-list/container APIs, exact error/logger behavior and node lifecycle integration remain incomplete. Preference-byte persistence here is not a native H2 database engine.

Pinned Java SHA-256: `491620435babecb9f6f837308e6614694759e8884e19e9a0196389c35c226093`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
