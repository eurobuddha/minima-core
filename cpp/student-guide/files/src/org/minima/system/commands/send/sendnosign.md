# sendnosign — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ command compiles; complete node and API parity remain unfinished.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/sendnosign.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendnosign.java) | [src/org/minima/system/commands/send/sendnosign.cpp](../../../../../../../../src/org/minima/system/commands/send/sendnosign.cpp) |

## What this file does

Construct an unsigned TxPoW with input proofs and scripts and export it for later offline signing.

## Follow the logic

1. Parse single or multi recipients and total, reject negative burn and accept split between one and twenty without flooring it. Unlike send, dryrun is an accepted but unread parameter.
2. Move the local tip backward by the confirmation depth, returning status:false if history runs out. Read coinage with default zero even though coinage is absent from getValidParams; normal runner validation and direct calls can differ.
3. Search relevant unspent coins at that older tip, apply age filtering and reuse send.selectCoins. Filter mining/mempool entries again while accumulating amounts; reject non-simple token scripts or insufficient funds via status:false.
4. Build inputs and choose an MMR proof node relative to the already-rewound tip, capped by proof history. Attach CoinProofs and wallet ScriptProofs, collecting unique public keys only for logging; no signing loop follows.
5. Validate total Minima/token precision. Divide each recipient amount by the original split value and loop split.getAsInt() times, always storing state. No extra remainder output is constructed; fractional split and rounding therefore differ from send.
6. Add positive change to a default wallet address without storing state. State values are cast to String rather than generally stringified. Precompute output IDs and calculate the transaction ID.
7. For token burn, call createBurnTransaction with its signing Boolean false. Generate/recalculate TxPoW, write the requested file or unsignedtransaction-<time>.txn and return its absolute path. This method does not enforce send’s output-count/TxPoW-size checks or start mining.

## How the C++ translation works

The C++ command reuses send.selectCoins and the existing recipient tokenizer, token checks, wallet, MMR proofs, generator and MiniFile. The tokenizer and required-token helper were moved unchanged from send.cpp into a shared header. The source-specific flow remains in this mapped file: ancestor rewind, fractional splitting, strict string state, unsigned burn and file output are not delegated to send's newer algorithm.

Native shared transaction and witness owners are passed to the real generator. File output uses the actual complete-TxPoW serializer and real clock. The confirmation depth is captured once before walking parents, matching the Java local integer. Null state data retains the StateVariable rule that port validation happens before dereferencing the string.

## What the tests tell us

The [unsigned-file lab](../../../../../../../new-tests/unsigned-transaction-files.md) uses twelve-node MMR fixtures, plain/custom tokens and recording SQL. It compares replies, metadata, logs, selected coins, proofs, outputs, recursive files and directory effects. Sequences create, view, sign, verify and post the actual files. A custom-token case independently checks that both witnesses start unsigned and that the burn links to the main transaction ID; subsequent signing verifies both signatures.

The final 309 programs pass in both normal and ASAN/UBSAN builds, with 1,248 operations and 20,081 recorded SQL calls each. All seven selected checks pass per build. Consult the lab for measured results and the corrected zero-total expectation. Explicit expectations include amount 3 with split 1.5 producing one amount-2 output, zero total amounts being rejected, a zero recipient within a positive-total multi-send, dryrun still producing a file, and numeric state causing ClassCastException. These assertions explain what the differential comparison is intended to detect.

Each file is decoded using the real header and TxPoW readers. The observer records the actual offset/length of its 32-byte random PRNG and checks that the decoded value matches that exact wire slice. Only that slice is normalized; every other byte remains compared. Generated clock filenames are shape-checked before normalization. This is narrower than ignoring complete file contents or transaction hashes.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxPoWDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java), [TxPoWTreeNode](../../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [TxnRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [ScriptRow](../../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java), [Wallet](../../../../../../../../../upstream-study/src/org/minima/database/wallet/Wallet.java), [Address](../../../../../../../../../upstream-study/src/org/minima/objects/Address.java), [Coin](../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [CoinProof](../../../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java), [ScriptProof](../../../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java), [StateVariable](../../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java), [Token](../../../../../../../../../upstream-study/src/org/minima/objects/Token.java), [Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [TxPoW](../../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [Witness](../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MMRProof](../../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [TxPoWGenerator](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java), [TxPoWMiner](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java), [TxPoWSearcher](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [txnutils](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnutils.java), [GlobalParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [AddressAmount](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendnosign.java#L47), [getAddress](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendnosign.java#L52), [getAmount](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendnosign.java#L56), [sendnosign](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendnosign.java#L61), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendnosign.java#L66), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendnosign.java#L72), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendnosign.java#L131), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/sendnosign.java#L570).

## Check your understanding

With split 1.5, why can the number of outputs and the divisor disagree? Which remainder-handling step is absent?

## Limits and traceability

Main startup and durable native SQL remain absent. Recording SQL rows do not prove persisted key-use counters, and a retained miner queue is not network acceptance. The native AddressAmount uses value fields; arbitrary null/reference API behaviour and concurrent global mutation remain unverified. This command is still partial and full parity remains not_verified.

Pinned Java SHA-256: `841b5de96dab16d616efa25df90a8e6418e37788e73eb56b5a350501d7deae07`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
