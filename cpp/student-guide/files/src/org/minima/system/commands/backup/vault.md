# vault — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/vault.java) · [Native implementation](../../../../../../../../src/org/minima/system/commands/backup/vault.cpp)

## Purpose

vault manages seed visibility, removal, restoration and password encryption, plus diagnostic derivation and full key-reset actions.

## Follow the command

1. seed returns the stored phrase/seed and locked state. wipekeys requires key creation complete and an unlocked wallet, verifies the supplied seed equals the stored seed, then wipes private material while retaining public records.
2. restorekeys cleans the phrase with BIP39, derives its seed and asks Wallet.resetBaseSeedPrivKeys to rebuild private keys, reporting original/cleaned text. passwordlock also requires completed keys, rejects semicolons and optionally checks confirmation.
3. passwordLockDB rejects an already locked wallet, encrypts phrase bytes using PasswordCrypto, stores/saves the encrypted seed in UserDB, then wipes the wallet seed/private keys. Failure during wiping occurs after the encrypted copy was saved.
4. passowrdUnlockDB rejects an unlocked wallet, decrypts stored data and maps decryption exceptions to Incorrect password. It derives the seed, rebuilds private keys, then clears/saves the encrypted seed. A later save failure can leave the wallet unlocked.
5. testphrase derives default TreeKeys for modifiers 0 through numkeys−1 and returns their public keys/addresses without storing them. resetkeys stops services, wipes transaction/archive and node state files, resets the wallet phrase, creates default keys, sets all uses (default 100), saves SQL and stops Main.
6. checkAllKeysCreated only checks Main’s completion flag. stopAllKeysCreated requests the wallet stop creating new keys and sleeps two seconds when incomplete; it does not join or verify worker termination.

## Translation and dependencies

The native command reuses Command parsing/replies, Wallet, BIP39, PasswordCrypto, TreeKey, MiniFile and MinimaDB. It preserves save-before-wipe and clear-after-rebuild ordering. Main lifecycle methods use an explicit receiver whose absent default fails; native startup still has to bind it. Phrase comparison extracts the already tested Unicode folding helper used by unresolved socket-address equality.

## Understanding the tests

The fixture uses synthetic phrases and real encryption, then compares seed state, saved preference bytes, SQL calls, logger messages, Main calls and state-file deletion after each operation. Random ciphertext is compared by length and recovered plaintext; ciphertext produced by each language is also decrypted by the other. Checked and unchecked SQL failures show which earlier effects survive. The recording driver returns supplied rows, so it cannot establish durable database restoration.

- [New experiment](../../../../../../../new-tests/vault-and-signing.md)
- [Actual Java commands and recording SQL](../../../../../../../../support/WalletCommandsOracle.java)
- [Native command fixture](../../../../../../../../support/wallet_commands.cpp)
- [Test inputs](../../../../../../../../tools/vault_signing_corpus.py)
- [Comparison and assertions](../../../../../../../../tools/wallet_commands_differential.py)

Normal and ASAN/UBSAN comparisons both pass all 247 programs. The experiment page records the evidence and limitations.

## Limits

Only synthetic fixture seed material is used. These operations are not one database transaction, and the command’s diagnostic key generation is not a signing-state audit. testphrase returns seed material by design; fixtures should use synthetic phrases.

## Check your understanding

Why is saving the encrypted phrase before wiping private keys necessary, and what remains if the wipe fails?

Pinned Java SHA-256: `38d1cb311f72855ec1b1e574fd399c12a7f88b560f1a60a8713d62260af0894e`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).

## Backup/checker interruption follow-up

stopAllKeysCreated now uses NativeThread and ignores only InterruptedException after setting STOP_NEWKEYS. When keys are already complete, it returns before sleeping and leaves interruption pending. New runtime cases compare both branches and the actual wallet stop flag without opening wallet storage. See the [interruption lab](../../../../../../../new-tests/backup-checker-interruption.md) for results and remaining limits. Full parity stays not_verified.
