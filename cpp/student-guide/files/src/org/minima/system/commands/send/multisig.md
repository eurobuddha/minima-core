# multisig — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `unported`; parity `not_verified`. The mapped C++ file is absent. This page does not describe a completed translation.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/multisig.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java) | `src/org/minima/system/commands/send/multisig.cpp` (planned) |

## What this file does

Create, discover and spend coins protected by the fixed root-or-threshold multisignature contract, using custom transaction commands for file workflows.

## Follow the logic

1. Derive the address from the exact contract text. Previous state 1 is an optional root key (0x21 disables it), state 2 the required count, state 3 the key count and states 4 onward the keys. The script first accepts a root signature, otherwise builds and executes a MULTISIG expression.
2. Create optionally hashes a MiniString ID into state 0, checks root/public-key prefix and string length, and rejects fewer keys than required.getAsInt(). It does not independently require a positive integral threshold or validate every hexadecimal digit at this stage. Delegate funding to send and translate its status/message into the outer reply.
3. getkey returns the wallet default address’s public key. list searches the contract address and, when id is supplied, matches its hash against every state value rather than only port zero. A coin can appear more than once if several values match.
4. Spend finds the first matching coin for an id or loads explicit coinid, computes token-scaled change and rejects an equal recipient/source address. It builds txncreate/input/payment commands; positive change copies all old state and emits a state-storing return output. It exports and deletes the temporary row, returning the entire child-result array.
5. Sign imports a file and checks import status, then examines every state value of the first input for a wallet key. It queues a txnsign for every found key, exports to signed_<original filename> and deletes. Optional vault unlock/relock surrounds execution without finally protection.
6. Post imports, invokes txnpost mine:true auto:true and deletes. View imports then deletes, returning import details in the child results. Unknown action throws. The accepted mine parameter does not change the hard-coded post mining setting.
7. Generated command text preserves source quoting and separator behavior. The unused getRequiredFile helper simply resolves file; it does not add validation to these branches.

## How the translation is approached

No native command counterpart exists. A future port should retain the pinned Command/CommandRunner parsing and reply model and reuse compatible transaction, JSON and stream primitives. Required wallet/database/miner bindings and failure ordering must be implemented explicitly. Java command strings here are executable parser input, so quoting, separators and ignored child statuses are observable behavior.

## What the tests tell us

No direct command parity experiment was found in the inspected harnesses. Proposed future test: Use deterministic contract hashing/state fixtures, including duplicate matching state values and malformed key strings that pass the length check. Record multi-command text, first-input key selection, child-failure propagation and file-name placement. Verify state-preserving change separately from state-free payment. A later VM fixture must test root bypass and threshold satisfaction using real signatures. This experiment has not been implemented or run for this lesson; constituent codec or runner tests do not establish command coverage.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxPoWTreeNode](../../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [TxnDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java), [TxnRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [KeyRow](../../../../../../../../../upstream-study/src/org/minima/database/wallet/KeyRow.java), [ScriptRow](../../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java), [Wallet](../../../../../../../../../upstream-study/src/org/minima/database/wallet/Wallet.java), [Address](../../../../../../../../../upstream-study/src/org/minima/objects/Address.java), [Coin](../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [StateVariable](../../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java), [Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MiniString](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [TxPoWSearcher](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [CommandRunner](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [vault](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/vault.java), [Crypto](../../../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [multisig](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java#L43), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java#L48), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java#L120), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java#L126), [getRequiredFile](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java#L489), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java#L501).

## Check your understanding

If the ID hash occurs in two different state ports of one coin, how many entries can list return?

## Limits and traceability

The command counterpart is absent and parity remains not_verified. This is an authored source lesson and a proposed verification design, not evidence that this command is implemented or that its tests pass.

Pinned Java SHA-256: `866f07feb5aabd9c38cee6cb15200edf57536ba545c8756e5209939ccb01a08c`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
