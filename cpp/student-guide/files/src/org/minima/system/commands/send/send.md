# send — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The native command compiles and its current comparison corpus passes in both builds.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/send.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java) | [native implementation](../../../../../../../../src/org/minima/system/commands/send/send.cpp) |

## What this file does

Select spendable coins, construct proofs and split outputs, sign a transaction and submit it for mining.

## Follow the logic

1. Parse either multi address:amount strings or one recipient, sum amounts and add burn to required Minima funds. Reject negative burn; floor split before checking its configured range. dryrun enables debug; state/storestate/mining/source-address/signkey options are read independently.
2. Require coinage at least the confirmation depth. Search the current tip for relevant or explicitly addressed token coins and filter by creation height. Scale custom-token requirements into underlying Minima units before selection.
3. orderCoins sorts the caller’s list descending in place, records first-seen address order, then builds a separate address-grouped result. That grouped result need not remain globally descending. selectCoins skips mining/mempool coins while searching, retains the last sufficient coin until the first insufficient coin, then returns that singleton. If none was found, it returns the entire ordered list, including entries skipped during the scan.
4. The caller filters mining/mempool entries again and accumulates enough spendable value. Non-simple token scripts or insufficient funds return status:false with a message. Build inputs, choose a proof node using the newest input height capped by MMR proof history, and attach each CoinProof and wallet ScriptProof.
5. Collect unique signing keys, then apply the explicit signkey override using Java reference inequality against the empty-string literal. This is a source identity quirk, not ordinary string-content comparison. Validate total precision before constructing recipient outputs.
6. For each recipient divide its underlying amount by split, require a positive share, emit that many outputs and add a positive rounding remainder as an extra output. Positive change first looks up and may validate a default wallet key even if fromaddress later overrides its destination. Change never stores state; output count is checked after construction.
7. Convert state keys to ports and values through Java string concatenation, precompute output coin IDs and calculate the transaction ID. Outside dryrun, optionally unlock the wallet and sign each required key. Relocking occurs on normal paths and explicit oversize rejection, not in a general finally block.
8. For token transfers with positive burn, call createBurnTransaction and generate a combined TxPoW; that helper call is outside the dryrun guard. Recalculate its ID and enforce the tip’s byte limit. Non-dryrun submits asynchronously or mines with a 120-second limit; dryrun returns JSON plus detailed byte sizes without this final submission.
9. The AddressAmount inner class retains recipient data. The demo main prints an orderCoins example; it is not a behavioral parity test.

## How the translation is approached

The translation reuses the existing native Command parsing, shared Coin handles,
Transaction/Witness owners, TxPoWSearcher, proof construction, Wallet, keys, vault,
and miner boundary from txnutils. Stable sorting retains equal-amount order. The
stock empty-signkey paths use a content check after tracing the source helper.
The recipient tokenizer skips empty colon fields, exactly as StringTokenizer does.

## What the tests tell us

The [coin-selection and funding lab](../../../../../../../new-tests/coin-selection-and-funding.md)
is now implemented. It compares both selector lists, alias mutations, logs,
reservations, funding state, replies and real transaction/proof/signature paths.
The first run exposed an incorrect test assumption: `bad` is valid hexadecimal,
so that address cannot demonstrate a parsing failure. Both runtimes accepted it.
The corrected experiment uses `nothex`. The final command corpus passes 736 programs, 3,418 operations and 39,016 recording SQL calls in normal and ASAN/UBSAN builds.

The raw-transaction addition moves stateText unchanged into [StateText.hpp](../../../../../../../../src/support/StateText.hpp), shared with the raw wrappers. It preserves the string/double/other-value conversion branches; no send control flow changes. The funding comparison passes 736 programs, 3,418 operations and 39,016 recorded SQL calls per build for this extraction. A later focused StateVariable Unicode correction also passes the raw and object regressions.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxPoWDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java), [TxPoWTreeNode](../../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [TxnRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [ScriptRow](../../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java), [Wallet](../../../../../../../../../upstream-study/src/org/minima/database/wallet/Wallet.java), [Address](../../../../../../../../../upstream-study/src/org/minima/objects/Address.java), [Coin](../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [CoinProof](../../../../../../../../../upstream-study/src/org/minima/objects/CoinProof.java), [ScriptProof](../../../../../../../../../upstream-study/src/org/minima/objects/ScriptProof.java), [StateVariable](../../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java), [Token](../../../../../../../../../upstream-study/src/org/minima/objects/Token.java), [Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [TxPoW](../../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [Witness](../../../../../../../../../upstream-study/src/org/minima/objects/Witness.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Signature](../../../../../../../../../upstream-study/src/org/minima/objects/keys/Signature.java), [MMRProof](../../../../../../../../../upstream-study/src/org/minima/objects/mmr/MMRProof.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [TxPoWGenerator](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java), [TxPoWMiner](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWMiner.java), [TxPoWSearcher](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [vault](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/vault.java), [keys](../../../../../../../../../upstream-study/src/org/minima/system/commands/search/keys.java), [txnutils](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnutils.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [GlobalParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [AddressAmount](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java#L51), [getAddress](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java#L56), [getAmount](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java#L60), [send](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java#L65), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java#L70), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java#L141), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java#L149), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java#L798), [selectCoins](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java#L810), [selectCoins](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java#L814), [orderCoins](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java#L900), [compare](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java#L907), [main](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/send.java#L937).

## Dependency follow-up

The native vault and keys commands now exist and have their own comparison experiments. Read the [vault/signing lab](../../../../../../../new-tests/vault-and-signing.md) and [wallet-key lab](../../../../../../../new-tests/wallet-keys.md) when studying send. Their tested helper behavior is reusable; this does not establish send command parity.

Trace the signkey string back through Command.getAddressParam before choosing a C++ check. An absent parameter returns the empty literal. A supplied blank fails in getParam. A supplied 0x converts to empty bytes, and BaseConverter.encode16 returns the empty literal for those bytes. Thus the stock helper's empty results come from literal-return paths; the new corpus explicitly compares absent, blank, 0x and nonempty signkey values. Java subclass overrides are a separate API question.

The dryrun byte-size report uses MiniData objects as JSONObject keys. JSONObject is a LinkedHashMap, so insertion order is relevant; conversion to key text happens during writing. MiniData overrides equals without overriding hashCode. A native string-only map therefore needs a separate review for distinct object keys with equal printed IDs. Do not assume that parsing the final JSON back into a string-key map can detect duplicate-key serialization differences.

## Check your understanding

For A:10, A:1, B:9 and requirement 8, which single coin is returned? Explain why the algorithm misses the smaller sufficient B coin.

## Limits and traceability

The native command and comparison corpus exist; full parity remains not_verified. Generic Java JSONObject object-key access, arbitrary subclass overrides, full Main startup and native durable SQL remain unverified. Searcher removes duplicate input IDs and precomputation assigns output IDs; the dry-run report currently emits their string keys. Distinct equal-ID object-key behavior is not certified.

Pinned Java SHA-256: `129d65b36bd9b914ee30cc9651d73a02d10fb24d1097252e8cf4a232eef3a861`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## Shared recipient parsing

The [unsigned-file lab](../../../../../../../new-tests/unsigned-transaction-files.md) reuses this command’s selector. Its existing nextRecipientToken and requiredToken helpers were moved unchanged to the shared SendRecipients header so both commands use the same tested implementation. send’s funding and output rules remain in this file; sendnosign has different confirmation, splitting, state and signing rules. The funding regression is rerun for the extraction.
