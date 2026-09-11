# backup — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/backup.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/backup/backup.cpp)

## Purpose

backup captures wallet and node state into a length-framed, compressed and encrypted file. Its response is stored under backup rather than the usual response key.

## Follow the command

1. Require all default keys created. If auto exists, set the UserDB flag; auto:false immediately returns that flag, while auto:true continues to make a backup. Choose a timestamped filename and default password minima, optionally checking confirm.
2. Create the output path, delete an existing file and make a temporary backup directory before taking the database read lock. Save state, export wallet SQL and save cascade, chain tree, user and P2P databases into temporary files.
3. Read those files into MiniData and serialize a capped relevant TxPoWList. Generate an eight-byte salt and cipher IV, writing both unencrypted as MiniData frames.
4. Derive the password key through GenerateKey. Layer GZIP output over cipher output over the file, then write six MiniData frames in this exact order: wallet, cascade, tree, user, P2P, TxPoWList. Thus data is compressed before encryption.
5. Close the streams, calculate size fields and include current block/auto state. Delete temporary files and release the read lock. The catch releases the lock, attempts temporary cleanup and wraps the exception; the partial destination file is not removed there.

## Translation and dependencies

The C++ command reuses MinimaDB stores, MiniData framing, GenerateKey, MiniFile,
MiniFormat and MiniUtil's date pattern. BufferedFileOutput gains unbuffered writes.
GzipOutput reuses the existing zlib settings but emits Java's header, 512-byte
compressed chunks and final CRC/length trailer. Its destructor releases zlib state;
normal completion explicitly finishes gzip, finalizes AES, and closes the file.
The command preserves source exception ordering, including wrapping an existing
CommandException inside another CommandException. Java Error and native allocation
failures bypass the ordinary catch, as the source requires.

## Understanding the tests

Read the [backup lab](../../../../../../../../student-guide/new-tests/backup.md),
[Java oracle](../../../../../../../../support/BackupOracle.java),
[native fixture](../../../../../../../../support/backup.cpp), and
[comparison driver](../../../../../../../../tools/backup_differential.py).

The fixtures run the real command with real H2 stores, using an unstarted Main
only to control key-readiness and avoid unrelated network workers. A copied Java
database baseline gives both runs identical wallet SQL and transaction history.
Ten deterministic transaction records include six relevant records. Both decoders
read each surviving encrypted file, and Java/native backup plaintext is compared
byte for byte across all six sections. The test separately checks saved state,
destination replacement, temporary-folder cleanup, lock counts, reply types,
password/confirm ordering, auto side effects, history limits and registry dispatch.
Random salt/IV are checked for framing; default filename timestamps must lie
inside their measured execution window. These values are not forced equal.

A second test stream records each gzip downstream write and injects IOExceptions.
It checks exact compressed bytes and write boundaries, with empty/random/repeated
payloads, finish called twice, and a write after finish. An independent gzip
reader must recover the original successful payload.

The first numeric-block mismatch taught us why a JSON string is different from a
JSON number. A later blank-history case exposed the source's nested
CommandException, which the port now retains. Both failures are saved in
[checkpoint evidence](../../../../../../../../validation/backup-work-in-progress).
JVM helpful-null and class-cast diagnostic prose is explicitly normalized;
exception categories and nesting remain checked.

## Limits

Complete file/API parity remains not_verified. Resource exhaustion, special-device
write/close failures, concurrent mutation, and mutable SimpleDateFormat APIs are
not certified by this fixture. The code reads all six sections into memory.
complete remains read but unused and absent from the valid parameter list.
The test exercises backup and decryption; restoring a running node is separate work.

## Check your understanding

Why must a reader decrypt before decompressing, and why are salt and IV outside that encrypted stream?

Pinned Java SHA-256: `c1eecca6c98a3ed875388934f4b40270962f51e78743170030b92f547e56ffc0`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
