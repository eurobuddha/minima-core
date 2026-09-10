# sendsign — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file compiles; complete API and node integration remain unfinished.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/sendsign.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendsign.java) | [src/org/minima/system/commands/send/sendsign.cpp](../../../../../../../../src/org/minima/system/commands/send/sendsign.cpp) |

## What this file does

Add wallet script proofs and signatures to both transactions in an exported TxPoW, then write a new signed file.

## Follow the logic

1. Read/decode the required TxPoW file before looking up Wallet. If a password is present and the seed is unavailable, unlock and remember to relock later.
2. Visit ordinary transaction inputs in order. Resolve each address to a wallet script row, reject a missing row, append its ScriptProof and sign the transaction ID with that row’s public key.
3. Repeat for burn-transaction inputs using the burn witness and burn transaction ID. The reqsigs list is checked but never populated: repeated public keys are signed again for each input.
4. Relock only after both loops complete, recalculate the TxPoW ID and write signedtransaction-<time>.txn in the base folder.
5. Return the absolute output filename under response.txpow. Relocking is not in finally; an earlier signing/script failure can bypass it. The demo main merely prints MiniData encodings of 0 through 511.

## How the C++ translation works

The C++ command reuses MiniFile/TxPoW, the actual Wallet signData method, ScriptProof, Witness and vault helpers. One loop over main then burn preserves the two Java loops’ order. The required-signature vector deliberately stays empty. Witness.addSignature independently discards duplicate root keys, so repeated signing calls need not produce additional witness entries. The output uses the real wall clock, actual TxPoW serialization and an absolute filename. The demonstration main reuses MiniData(BigInt) and prints all 512 source values; its ignored argument pointer accepts null.

## What the tests tell us

The offline-file fixture signs nine TxPoWs with zero to two main and burn inputs, verifies retained signatures against the correct transaction ID and compares complete output bytes. Repeated-key cases assert one signing/key-use write per input but one retained signature per nonempty transaction witness. Recording SQL rows always supply uses=0; this checks call counts, not a durable incrementing database. Missing/null script/key rows, exhausted keys and 60 injected SQL failures exercise partial effects. Locked/unlocked password cases use the real vault cryptography. A file used as the output folder must fail after signing, without creating an output file. The demo comparison captures all 512 printed lines with null and empty arguments.

See the [experiment and measured results](../../../../../../../new-tests/offline-send-files.md) and [actual corpus](../../../../../../../../tools/offline_send_corpus.py).

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [ScriptRow](../../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java), [Wallet](../../../../../../../../../upstream-study/src/org/minima/database/wallet/Wallet.java), [Coin](../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [ScriptProof](../../../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java), [Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [TxPoW](../../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [Witness](../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [Signature](../../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [vault](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/vault.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [sendsign](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendsign.java#L26), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendsign.java#L31), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendsign.java#L36), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendsign.java#L65), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendsign.java#L176), [main](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendsign.java#L180).

## Check your understanding

Why would adding pubkey to reqsigs fix a likely intention but break a literal translation?

## Limits and traceability

Full native Main startup and durable SQL storage remain absent. The fixture compares isolated command behaviour, not network acceptance or every inherited Java API. Filename clocks and independently random vault ciphertext are normalized as described in the lab. Full parity remains not_verified.

Pinned Java SHA-256: `08585ee2dcc253770a21fa0bf6b01280f448222e255abd3defc4485266f62716`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
