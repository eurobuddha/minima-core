# backup — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/backup.java) · `src/org/minima/system/commands/backup/backup.cpp` (absent)

## Purpose

backup captures wallet and node state into a length-framed, compressed and encrypted file. Its response is stored under backup rather than the usual response key.

## Follow the command

1. Require all default keys created. If auto exists, set the UserDB flag; auto:false immediately returns that flag, while auto:true continues to make a backup. Choose a timestamped filename and default password minima, optionally checking confirm.
2. Create the output path, delete an existing file and make a temporary backup directory before taking the database read lock. Save state, export wallet SQL and save cascade, chain tree, user and P2P databases into temporary files.
3. Read those files into MiniData and serialize a capped relevant TxPoWList. Generate an eight-byte salt and cipher IV, writing both unencrypted as MiniData frames.
4. Derive the password key through GenerateKey. Layer GZIP output over cipher output over the file, then write six MiniData frames in this exact order: wallet, cascade, tree, user, P2P, TxPoWList. Thus data is compressed before encryption.
5. Close the streams, calculate size fields and include current block/auto state. Delete temporary files and release the read lock. The catch releases the lock, attempts temporary cleanup and wraps the exception; the partial destination file is not removed there.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse GenerateKey parameters, MiniData framing and each database’s existing export format. Native streaming/compression/encryption must produce data the pinned restore can consume; exact random ciphertext equality requires controlled salt/IV.

## Understanding the tests

The runner fixture checks backup’s permission category only. Proposed format tests fix randomness, compare headers/section bytes and restore both Java-produced and native-produced files in isolated databases. Inject each export/write/close failure, assert lock release and partial-file state, and test auto:false before password/file validation.

- [Runner fixture using selected fake commands](../../../../../../../../support/CommandRunnerOracle.java)
- [Native runner fixture](../../../../../../../../support/command_runner.cpp)
- [Runner-only dispatch/permission coverage](../../../../../../../../tools/command_runner_differential.py)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

No backup or wallet action was performed. The body reads complete component files into memory and provides no atomic destination replacement. complete is read but unused and absent from its valid list.

## Check your understanding

Why must a reader decrypt before decompressing, and why are salt and IV outside that encrypted stream?

Pinned Java SHA-256: `c1eecca6c98a3ed875388934f4b40270962f51e78743170030b92f547e56ffc0`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
