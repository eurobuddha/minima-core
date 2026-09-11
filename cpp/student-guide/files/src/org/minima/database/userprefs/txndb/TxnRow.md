# TxnRow — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/database/userprefs/txndb/TxnRow.java](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java) | [src/org/minima/database/userprefs/txndb/TxnRow.cpp](../../../../../../../../src/org/minima/database/userprefs/txndb/TxnRow.cpp) |

## What this file does

Keep a draft transaction ID, its Transaction and its Witness as one serializable record.

## Follow the logic

1. Store the ID and object references. setID renames the same row; clearWitness replaces its Witness object rather than clearing the old object in place.
2. toJSON first includes id and precomputes transaction coin IDs, then serializes the transaction. When showWitness is true, include witness JSON and serialized output coins.
3. Write fields in order: MiniString ID, Transaction, Witness.
4. Read ID first, replace/read a new Transaction next, and only then replace/read a new Witness. If transaction reading fails, the old witness can still remain.
5. The convenience conversion catches IOException and returns null after logging; direct read exposes partial state.

## How the translation is approached

C++ uses optional ID plus shared transaction/witness ownership and sequential replacement in readDataStream. It preserves JSON’s coin-ID side effect and showWitness behavior. The convenience helper now routes caught IOException through MinimaLogger and NativeThrowable, matching the existing Coin conversion pattern. It still returns null after that log.

## What the tests tell us

The fixtures compare complete and partial row reads, byte offsets, prior transaction/witness state, showWitness variants, row renaming and clearWitness. The same records feed offline command import/export cases, linking the wire representation to user operations.

The complete [TxnDB driver](../../../../../../../../tools/txndb_differential.py), [Java oracle](../../../../../../../../support/TxnDBOracle.java) and [native harness](../../../../../../../../support/txndb.cpp) compare source-derived serialized fixtures, partial byte consumption and persistent operation sequences. The existing [normal report](../../../../../../../../validation/streamable-txbody-work-in-progress/normal/txndb-differential-results.json) records 2,503 programs passed and zero failures. I/O failures and other exceptions are distinguished only as IO_ERROR versus ERROR, so fine-grained error text/classes and logger behavior are not established. That older report does not establish logger parity. The newer mining-command suite exercises truncated row conversion and compares captured exception-log bodies; logger prefixes and stack frames are excluded.

## Dependencies and reading map

Direct Minima imports: [Coin](../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [Witness](../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniString](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [TxPoWGenerator](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [TxnRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java#L26), [TxnRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java#L28), [setID](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java#L34), [getID](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java#L38), [getTransaction](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java#L42), [getWitness](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java#L46), [clearWitness](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java#L50), [toJSON](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java#L54), [toJSON](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java#L58), [convertMiniDataVersion](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java#L86), [writeDataStream](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java#L108), [readDataStream](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java#L115), [ReadFromStream](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java#L125).

## Check your understanding

At a truncation halfway through Transaction, which fields have been replaced and which can still refer to the old objects?

## Limits and traceability

Complete generic exception, logger, public reference-mutation and stream-overload parity remain open; successful round trips alone do not establish them.

Pinned Java SHA-256: `8865cb415846d2581fa3c174c5c6a9c2f2b9a2d1b8dc098d5a7380eac4acbc3d`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
