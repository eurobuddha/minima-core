# multisig — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped native command compiles; finite workflow checks do not certify complete API or node parity.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/send/multisig.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java) | `src/org/minima/system/commands/send/multisig.cpp` |

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

The mapped `.cpp` keeps the source name, constructor, help, accepted parameters and fresh-command factory. Both command files call `support/MultisigCommands.cpp` for their shared action logic. The exact contract in `support/MultisigContract.hpp` is also used by Wallet. Reusing this literal matters: changing script text can change the address that identifies a protected coin.

The shared function takes the command object and a read-mode flag. Read mode removes create/sign and preserves the source's plain-string spend address. Full mode uses the existing address parser. Search, state handling, command parsing, file serialization and signing use the existing native implementations; the port does not substitute recorded replies for production children.

## What the tests tell us

The [workflow lab](../../../../../../../new-tests/multisig-workflow.md) explains `tools/multisig_differential.py`. Java constructs coins and valid MMR histories, then both runtimes execute the same programs. A thin test-only wrapper records child command text and still runs the real child. The comparison includes replies, SQL effects, custom rows and exported file bytes.

For example, putting the ID hash at ports 0 and 9 makes list return the same coin twice; spend selects the first matching coin once. A payment discards old state while positive change copies it. Repeated public-key values cause repeated signing attempts, while the witness handles duplicate signatures according to its own rules. Parent status and each nested status must be inspected separately.

Generated 32-byte transaction IDs are shape-checked. Only their complete serialized MiniString fields and textual references are normalized; transaction and witness bytes remain compared. Clock-generated filenames are also checked before normalization. Malformed-file testing found an older txnimport exception mismatch, corrected to retain Java's null-pointer exception and argument-evaluation order. Reports retain the original failure.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxPoWTreeNode](../../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [TxnDB](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnDB.java), [TxnRow](../../../../../../../../../upstream-study/src/org/minima/database/userprefs/txndb/TxnRow.java), [KeyRow](../../../../../../../../../upstream-study/src/org/minima/database/wallet/KeyRow.java), [ScriptRow](../../../../../../../../../upstream-study/src/org/minima/database/wallet/ScriptRow.java), [Wallet](../../../../../../../../../upstream-study/src/org/minima/database/wallet/Wallet.java), [Address](../../../../../../../../../upstream-study/src/org/minima/objects/Address.java), [Coin](../../../../../../../../../upstream-study/src/org/minima/objects/Coin.java), [StateVariable](../../../../../../../../../upstream-study/src/org/minima/objects/StateVariable.java), [Transaction](../../../../../../../../../upstream-study/src/org/minima/objects/Transaction.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MiniString](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [TxPoWSearcher](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [CommandRunner](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandRunner.java), [vault](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/vault.java), [Crypto](../../../../../../../../../upstream-study/src/org/minima/utils/Crypto.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [multisig](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java#L43), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java#L48), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java#L120), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java#L126), [getRequiredFile](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java#L489), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/send/multisig.java#L501).

## Check your understanding

If the ID hash occurs in two different state ports of one coin, how many entries can list return?

## Limits and traceability

The counterpart exists but remains implemented_partial/not_verified. Recording SQL does not establish durable storage; local chain, file and miner fixtures do not establish network acceptance. Review the current lab/evidence for the exact observed coverage. Full native-node and arbitrary reference/concurrency behavior remain unfinished.

Pinned Java SHA-256: `866f07feb5aabd9c38cee6cb15200edf57536ba545c8756e5209939ccb01a08c`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
