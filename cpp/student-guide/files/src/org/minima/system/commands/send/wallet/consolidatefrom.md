# consolidatefrom — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The native counterpart exists; full API and behavior review remain open.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/wallet/consolidatefrom.java](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/consolidatefrom.java) | [consolidatefrom.cpp](../../../../../../../../../src/org/minima/system/commands/send/wallet/consolidatefrom.cpp) |

## What this file does

Combine custom-address coins into ten equal outputs, or use the public helper to combine a supplied list into one output. Both paths delegate transaction operations to existing commands.

## Follow the logic

1. Read fromaddress, tokenid, maxcoins and burn, then script, privatekey and keyuses. A positive burn with a non-Minima token is rejected in the main command.
2. Generate the random ID and create the temporary transaction before parsing mine or reading the chain tip. Later validation failures can therefore leave the row behind.
3. Search actual unspent coins matching the address and token. Sort stably by descending Minima amount. Keep coins at least three blocks old, checking mempool membership only after the age condition succeeds.
4. If no valid coins remain, throw without deleting. Otherwise add inputs in sorted order, sum token amounts, and check maxcoins after each addition. Even a zero or negative limit selects one coin.
5. For a positive Minima burn, require burn strictly below the selected total. On rejection, delete before throwing. Divide the remaining total by ten using MiniNumber and issue ten output commands.
6. Add script, MMR proofs and custom signature; post using mine, delete and return posting.response. The wrapper does not check any returned child status.
7. The public createConsolidate helper is a separate algorithm: supplied input order, no chain filtering or limit, positive burn regardless of token, one output, mine:true, then deletion and a strict nullable JSONObject response cast.

## How the translation is approached

Reuse TxPoWSearcher and TxPoWDB/RamDB for real search and mempool lookup. The stable descending comparison matches the existing send implementation without its extra address grouping. CommandResult supplies first-child selection. C++ optional strings model Java null concatenation; nullable coin owners/list and number pointers preserve helper null failures at their original points. Integer increment uses explicit Java wraparound. No finally cleanup or new child-status check is added.

## What the tests tell us

[The consolidation corpus](../../../../../../../../../tools/consolidate_address_corpus.py), [the shared driver](../../../../../../../../../tools/wallet_wrappers_differential.py) and [the pre-test lab](../../../../../../../../new-tests/consolidate-custom-address.md) compare actual wrapper bodies. Fixture coins populate real chain nodes, MegaMMR and mempool witness records. Independent checks cover stable order, age boundaries, spent and duplicate IDs, mempool cascade flags, one-versus-ten outputs, burn failures, null helper arguments and response casting after deletion. Selected real transaction children additionally expose stored input/output bytes. The final consolidation corpus passes all 331 programs and 3,815 child calls in normal and ASAN/UBSAN builds (12.81s/22.38s). Both full C++ builds pass without warnings. The corpus includes 55-coin cutoff tests and truncation of the smallest unit across ten outputs.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxPoWDB](../../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java), [TxPoWTreeNode](../../../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [Coin](../../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [TxPoWSearcher](../../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java), [Command](../../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [CommandRunner](../../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [GeneralParams](../../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [JSONArray](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [consolidatefrom](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/consolidatefrom.java#L24), [getValidParams](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/consolidatefrom.java#L29), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/consolidatefrom.java#L35), [compare](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/consolidatefrom.java#L82), [createConsolidate](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/consolidatefrom.java#L183), [runCommand](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/consolidatefrom.java#L241), [getFunction](../../../../../../../../../../upstream-study/src/org/minima/system/commands/send/wallet/consolidatefrom.java#L248).

## Check your understanding

Explain why maxcoins:0 still adds one input. Then contrast a main-command empty search, a burn equal to total, a thrown signing call and a helper response of the wrong JSON type: which paths delete their temporary row?

## Limits and traceability

Counterpart implemented_partial and full parity not_verified. The fixture uses source-supported test chain nodes and controlled posting/signing replies; it does not prove funded network acceptance, durable SQL or complete native Main ownership. Equal-amount order follows search encounter order, including the collection order used by MegaMMR.

Pinned Java SHA-256: `a0d77a96c534c8dbd76010447dbfeab157dd7e3134a5575f0a71f0576baf6d47`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
