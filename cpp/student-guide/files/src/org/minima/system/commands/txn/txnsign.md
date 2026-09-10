# txnsign — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/txn/txnsign.java) · [Native implementation](../../../../../../../../src/org/minima/system/commands/txn/txnsign.cpp)

## Purpose

txnsign adds signatures to a stored transaction and can then post/delete it. It supports wallet auto-selection, one wallet key or a supplied private TreeKey.

## Follow the command

1. Require id/publickey and row, precompute coin IDs and transaction ID. If password is supplied and wallet locked, unlock it and remember to relock later.
2. auto examines input-address scripts, records missing and nonsimple addresses, and signs simple keys only when the witness is not already signed by them. Missing/nonsimple inputs do not themselves fail the command.
3. custom reads privatekey and keyuses, creates a default TreeKey, sets its use index and signs without wallet persistence; it does not add that public key to the response keys list. Explicit wallet-key mode requires a record and signs even if an existing signature is present.
4. Relock after normal signing, or in the special explicit missing-key branch. Other exceptions between unlock and this point have no finally relock, so failures during signing/parsing can leave the wallet unlocked.
5. Report newly used keys and optional nonsimple/notfound arrays. Presence of txnpostauto triggers posting even when its value is false; the boolean controls automatic proof/script setup. Read burn/mine and call txnpost.postTxn after relocking.
6. Only after posting, optionally delete the row and report delete. Without txnpostauto, report txnpost:false and ignore txndelete. A later failure does not remove signatures already added or restore key-use counters.

## Translation and dependencies

The native command reuses actual stored transaction/witness owners, Wallet and TreeKey signing, vault lock/unlock and txnpost. The translation follows the Java sequence directly: it does not add an automatic cleanup scope that would relock on every exception. Presence checks remain distinct from parsing a boolean value.

## Understanding the tests

The fixture signs real transactions with deterministic keys, compares complete witness bytes, and independently verifies stored signatures. Cases include repeated inputs, missing and nonsimple scripts, nullable key results, custom key-use indices, locked-wallet transitions, posting and late deletion errors. An initial negative-index mismatch exposed a generic C++ vector exception; TreeKeyNode now reports the Java array-bounds category and index. A short-chain posting fixture correctly failed in both languages; the successful auto-post cases now reuse the existing twelve-block chain.

- [New experiment](../../../../../../../new-tests/vault-and-signing.md)
- [Actual Java commands and recording SQL](../../../../../../../../support/WalletCommandsOracle.java)
- [Native command fixture](../../../../../../../../support/wallet_commands.cpp)
- [Test inputs](../../../../../../../../tools/vault_signing_corpus.py)
- [Comparison and assertions](../../../../../../../../tools/wallet_commands_differential.py)

Normal and ASAN/UBSAN comparisons both pass all 247 programs. The experiment page records the evidence and limitations.

## Limits

Synthetic private keys produce real signatures in the fixture. Successful auto signing does not mean every input was signed, and a supplied custom key-use value is not tracked persistently here.

## Check your understanding

Why can txnpostauto:false still post the transaction?

Pinned Java SHA-256: `8a1718ddec2f877d6cfc93a572c5b6e606f7f068e760377892729a06c6f7eb3f`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
