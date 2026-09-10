# Wallet locking and transaction signing: order changes the outcome

Read [vault.java](../../../upstream-study/src/org/minima/system/commands/backup/vault.java) and [txnsign.java](../../../upstream-study/src/org/minima/system/commands/txn/txnsign.java).

## Password locking is a sequence of effects

Locking first encrypts the current phrase with the existing PasswordCrypto format, stores the ciphertext in UserDB, saves UserDB, and only then wipes private keys. An error in a later SQL call can leave earlier effects in place. Unlocking rejects an already unlocked wallet, decrypts the saved bytes, derives the seed from the recovered phrase without cleaning it again, restores private keys, clears the encrypted-seed field and saves preferences.

A command-side passwordlock checks key creation, rejects a semicolon and checks confirmation. The static passwordLockDB helper does not perform those command-only checks. Distinguish callers before moving validation into a shared helper.

Resetkeys asks Main to prepare for restoration, closes/wipes specified stores and removes specified state files, resets the wallet, creates default keys, advances key-use counts, saves SQL and marks/stops Main. The native Main receiver is an explicit unfinished integration boundary; fixtures record these calls in isolated temporary directories.

## Signing can fail after changing state

Signing precomputes coin IDs and calculates the transaction ID before accessing the wallet. Auto mode looks up each input script, reports missing and nonsimple addresses, and avoids keys already in the witness. Explicit-key mode looks up the supplied public key. Custom mode constructs a TreeKey from supplied private bytes and the specified use count.

A password is used only when supplied and the base seed is unavailable. Successful signing relocks before optional posting. The missing explicit-key branch also relocks. There is no general finally block: other failures can leave the wallet unlocked. Preserve and test that ordering instead of assuming every error rolls back work.

The presence of txnpostauto triggers posting even when its value is false; the value controls proof/script construction. Deletion is parsed and applied only after posting succeeds. It is ignored when posting was not requested.

## Reuse and evidence

The port uses actual Wallet, PasswordCrypto, BIP39, TreeKey, Witness and txnpost code. Ciphertext is random: compare recovered plaintext and cross-decryption rather than requiring independently encrypted bytes to match. SQL calls use the existing recording driver, so these tests do not establish durable H2 storage. Main lifecycle calls remain explicit recorded boundaries.

The Witness nullable-signature entry point must reject null with the source IllegalArgumentException. A null public key still walks existing signature keys before returning false. Case-insensitive phrase comparison reuses the existing Unicode fold from unresolved socket-address equality.

## What the first runs revealed

Custom keyuses=-1 reached a negative index in TreeKeyNode. Java threw an array-bounds exception; C++ reported a generic vector error. The actual key/child accessors now reuse the existing Java exception adapter. Twelve direct two-key-node cases also test both bounds, INT_MIN and INT_MAX. TreeKey exhaustion logging now goes through the actual MinimaLogger.

A posting assertion failed even though Java and C++ agreed: the supplied chain was too short for automatic proofs. That source rejection remains tested. Successful automatic posting now reuses the existing twelve-block wallet chain; all eight combinations of automatic proof setup, synchronous mining and deletion succeed in the source fixture. Posting success here is not external network acceptance.

The normal suite passes 247 programs, 800 operations and 16,275 recording SQL calls, including eight cross-language ciphertext-decryption programs. Stored signatures are independently verified, including successful custom-key cases. The sanitizer run first exceeded the harness's 30-second response limit for a full-size custom tree; the optional response limit is now 180 seconds for this group. The sanitizer retry passes the same 247 programs, 800 operations and 16,275 SQL calls in 560.21 seconds, with no sanitizer diagnostic. All 18 selected regressions pass in both normal and sanitizer builds.

No complete-node claim is made.
