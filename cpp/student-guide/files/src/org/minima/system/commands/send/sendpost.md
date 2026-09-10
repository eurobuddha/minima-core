# sendpost — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file compiles; complete API and node integration remain unfinished.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/sendpost.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendpost.java) | [src/org/minima/system/commands/send/sendpost.cpp](../../../../../../../../src/org/minima/system/commands/send/sendpost.cpp) |

## What this file does

Read a TxPoW file, calculate its ID and hand it to asynchronous mining.

## Follow the logic

1. Resolve/read the required file and decode its complete bytes as a TxPoW.
2. Call calculateTXPOWID before constructing the reply.
3. Place the object JSON under response.txpow, then call mineTxPoWAsync with that same object.
4. Return only if the miner call returns. This method has no direct signature-validation or MMR-age check; downstream components determine acceptance.

## How the C++ translation works

The C++ command owns the decoded TxPoW through shared_ptr, recalculates its ID, snapshots JSON, and passes that owner to the existing txnutils miner receiver. The miner operates on that same retained object. Building JSON after mining would change the reply because the miner updates the body hash.

## What the tests tell us

The offline-file fixture compares the original reply snapshot, miner entry bytes, retained object bytes, queued message bytes and reservations. Independent assertions require one asynchronous call, one pending message and a changed serialized body after miner entry. It also posts files signed by sendsign. Missing Main or null miner, malformed files and permission/parameter failures are compared without launching a network node.

See the [experiment and measured results](../../../../../../../new-tests/offline-send-files.md) and [actual corpus](../../../../../../../../tools/offline_send_corpus.py).

## Dependencies and reading map

Direct Minima imports: [TxPoW](../../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [sendpost](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendpost.java#L15), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendpost.java#L20), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendpost.java#L25), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendpost.java#L45), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendpost.java#L75).

## Check your understanding

Why is the help text’s approximate proof-validity window not an implemented age check in this file?

## Limits and traceability

Full native Main startup and durable SQL storage remain absent. The fixture compares isolated command behaviour, not network acceptance or every inherited Java API. Filename clocks and independently random vault ciphertext are normalized as described in the lab. Full parity remains not_verified.

Pinned Java SHA-256: `3a08257a2ae476ecb320199329675be6b14389a3bbe1d8969589afe9ba606c46`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
