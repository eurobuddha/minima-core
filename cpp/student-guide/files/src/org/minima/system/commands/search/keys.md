# keys — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/search/keys.java) · [Native implementation](../../../../../../../../src/org/minima/system/commands/search/keys.cpp)

## Purpose

keys lists wallet key records, derives standalone keys, checks stored public/private correspondence or requests background key creation.

## Follow the command

1. list optionally normalizes publickey through MiniData and reads modifier text. A public-key filter takes precedence over modifier; either filter stops after its first match. Without a filter return all records. Compute maxuses only across selected records, starting at zero.
2. genkey uses a supplied phrase or generates a BIP39 word list, derives its seed, hashes it with BigInteger-zero modifier data and creates a default TreeKey. Return the phrase, private/public keys, modifier zero and SIGNEDBY script addresses without storing the key.
3. checkkeys first requires an available base seed. Rebuild each TreeKey from its stored private key, size and depth, compare its public key and log/count correct versus wrong records.
4. new delegates persistent key creation to the wallet. createallkeys reads keyuses and posts MAIN_LOAD_ALL_KEYS, returning Keys being created before that asynchronous job completes.
5. The static checkKey first asks the wallet to verify private seed/modifier derivation, then rebuilds the tree and compares public keys. It has no missing-record guard before dereferencing the key row. Unknown actions throw.

## Translation and dependencies

The native command reuses actual Wallet, KeyRow, BIP39, Crypto, TreeKey and Address implementations. An explicit Main message callback follows the existing txnminepost boundary and fails when no receiver is installed. Command.getParam trims the phrase first. The resulting text goes to seed derivation without BIP39 word cleaning or case normalization.

## Understanding the tests

The fixture compares list/filter behavior, new-key creation, seed/private/public checks, deterministic genkey, typed parameters and posted key-use counts. Java derives a synthetic private key from base seed 0xAA and modifier 0x00, then derives its actual public root; the native helper must accept this consistent record and reject a changed modifier. checkkeys also receives a mixed correct/incorrect public-key list.

The first differential failure was a null public-key field during filtering. Java failed on the null String receiver; C++ exposed bad_variant_access. The command now explicitly checks the nullable receiver before using the existing string accessor. A public-key filter still takes precedence over a null modifier filter.

- [Experiment](../../../../../../../new-tests/wallet-keys.md)
- [Java fixture](../../../../../../../../support/WalletCommandsOracle.java)
- [Native fixture](../../../../../../../../support/wallet_commands.cpp)
- [Corpus](../../../../../../../../tools/wallet_key_corpus.py)
- [Comparison driver](../../../../../../../../tools/wallet_commands_differential.py)

The corrected normal and ASAN/UBSAN comparisons both pass 132 programs, 262 operations and 7,895 recorded SQL calls.

## Limits

Only synthetic fixture keys are used. SQL is recorded, not durably executed. Random no-phrase genkey and the actual Main key-creation worker remain outside this command comparison. Comparing rebuilt public keys is not the same as validating signing-use counters or the complete seed derivation; the static helper performs an additional check.

## Check your understanding

Why does checkkeys differ from static checkKey even though both reconstruct a TreeKey?

Pinned Java SHA-256: `468cf13ff7be897e9cf765d5da01f96d15b413a9fc00cb0dcb144a9a95f7eb13`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
