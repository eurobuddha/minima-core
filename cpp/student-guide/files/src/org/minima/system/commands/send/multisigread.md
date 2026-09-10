# multisigread — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `unported`; parity `not_verified`. The mapped C++ file is absent. This page does not describe a completed translation.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/multisigread.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java) | `src/org/minima/system/commands/send/multisigread.cpp` (planned) |

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

No native command counterpart exists. A future port should retain the pinned Command/CommandRunner parsing and reply model and reuse compatible transaction, JSON and stream primitives. Required wallet/database/miner bindings and failure ordering must be implemented explicitly. Java command strings here are executable parser input, so quoting, separators and ignored child statuses are observable behavior.

## What the tests tell us

No direct command parity experiment was found in the inspected harnesses. Proposed future test: Compare shared branches with multisig while explicitly testing Mx versus hex recipient text, duplicate ID-state matches and unknown create/sign actions. Capture imported/deleted rows and the post request with a recording runner. Test permission classification separately; this source alone cannot prove public READ-mode authorization. This experiment has not been implemented or run for this lesson; constituent codec or runner tests do not establish command coverage.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxPoWTreeNode](../../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [ScriptRow](../../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java), [Address](../../../../../../../../../upstream-study/src/org/minima/objects/Address.java), [Coin](../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [StateVariable](../../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MiniString](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [TxPoWSearcher](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [CommandRunner](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [Crypto](../../../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [multisigread](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java#L37), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java#L42), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java#L93), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java#L99), [getRequiredFile](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java#L305), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisigread.java#L317).

## Check your understanding

Why can an Mx spelling of the source address pass this wrapper’s string inequality even when it denotes the same underlying address?

## Limits and traceability

The command counterpart is absent and parity remains not_verified. This is an authored source lesson and a proposed verification design, not evidence that this command is implemented or that its tests pass.

Pinned Java SHA-256: `136deeb440397397899089b5a9948a4106639293e21ccfa3b76b8c07403cda63`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
