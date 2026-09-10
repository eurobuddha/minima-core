# multisigread — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped native command compiles; finite workflow checks do not certify complete API or node parity.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/multisigread.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java) | `src/org/minima/system/commands/send/multisigread.cpp` |

## What this file does

Expose a reduced multisig command family for key lookup, coin listing, unsigned spend construction and file view/post operations.

## Follow the logic

1. Hash the same fixed root-or-threshold contract text as multisig and generate a random temporary ID before dispatching the requested action.
2. getkey reads the default wallet public key. list searches the multisig address; optional ID filtering checks all state values and can append duplicate copies of one coin.
3. Spend resolves an ID to the first matching coin or uses explicit coinid, calculates Minima/token-scaled change and rejects equal source/destination strings. Here address uses getParam, unlike multisig’s getAddressParam normalization.
4. Build payment with storestate:false. Positive change copies all previous state and emits storestate:true to the original address; export then delete and return all child results.
5. Post still invokes txnpost mine:true auto:true after import, and view imports/deletes. Thus the READ-oriented name does not mean every branch is side-effect free.
6. There are no create or sign branches, even though several accepted parameters relate to them. Unknown action throws, and the unused file helper adds no validation.

## How the translation is approached

The mapped `.cpp` keeps the source name, constructor, help, accepted parameters and fresh-command factory. Both command files call `support/MultisigCommands.cpp` for their shared action logic. The exact contract in `support/MultisigContract.hpp` is also used by Wallet. Reusing this literal matters: changing script text can change the address that identifies a protected coin.

The shared function takes the command object and a read-mode flag. Read mode removes create/sign and preserves the source's plain-string spend address. Full mode uses the existing address parser. Search, state handling, command parsing, file serialization and signing use the existing native implementations; the port does not substitute recorded replies for production children.

## What the tests tell us

The [workflow lab](../../../../../../../new-tests/multisig-workflow.md) explains `tools/multisig_differential.py`. Java constructs coins and valid MMR histories, then both runtimes execute the same programs. A thin test-only wrapper records child command text and still runs the real child. The comparison includes replies, SQL effects, custom rows and exported file bytes.

For example, putting the ID hash at ports 0 and 9 makes list return the same coin twice; spend selects the first matching coin once. A payment discards old state while positive change copies it. Repeated public-key values cause repeated signing attempts, while the witness handles duplicate signatures according to its own rules. Parent status and each nested status must be inspected separately.

Generated 32-byte transaction IDs are shape-checked. Only their complete serialized MiniString fields and textual references are normalized; transaction and witness bytes remain compared. Clock-generated filenames are also checked before normalization. Malformed-file testing found an older txnimport exception mismatch, corrected to retain Java's null-pointer exception and argument-evaluation order. Reports retain the original failure.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxPoWTreeNode](../../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [ScriptRow](../../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java), [Address](../../../../../../../../../upstream-study/src/org/minima/objects/Address.java), [Coin](../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [StateVariable](../../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MiniString](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [TxPoWSearcher](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [CommandRunner](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [Crypto](../../../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [multisigread](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java#L37), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java#L42), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java#L93), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java#L99), [getRequiredFile](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java#L305), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java#L317).

## Check your understanding

Why can an Mx spelling of the source address pass this wrapper’s string inequality even when it denotes the same underlying address?

## Limits and traceability

The counterpart exists but remains implemented_partial/not_verified. Recording SQL does not establish durable storage; local chain, file and miner fixtures do not establish network acceptance. Review the current lab/evidence for the exact observed coverage. Full native-node and arbitrary reference/concurrency behavior remain unfinished.

Pinned Java SHA-256: `136deeb440397397899089b5a9948a4106639293e21ccfa3b76b8c07403cda63`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
