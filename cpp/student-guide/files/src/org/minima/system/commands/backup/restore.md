# restore — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/restore.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/backup/restore.cpp)

## Purpose

restore replaces wallet and node databases from the six-section encrypted backup and normally stops the node for restart.

## Follow the command

1. Require all keys created and an existing file, read default password and shutdown switch. Load the whole backup, decode salt/IV, derive its cipher and open GZIP; only GZIP construction failures are relabeled Incorrect Password.
2. Call restoreReady(shutdown) before consuming all sections. Write wallet SQL to a temporary folder, disable state saving, and overwrite cascade, chain, user preferences and P2P files directly as each MiniData section is read.
3. Load those databases, clear uninstalled MiniDapp preferences, decode the relevant TxPoWList, wipe transaction SQL and insert each TxPoW as relevant. Close streams and re-enable state saving.
4. Restore wallet SQL and increment every key-use count by 256. Close transaction SQL, close/delete the archive database file, remove the temporary folder and save all databases.
5. Return restore.file and a restart message. When shutdown is true, mark shutdown complete, stop Main’s processor and notify its shutdown listener. There is no enclosing finally/rollback for section failure.

## Translation and dependencies

The native command reuses the backup/decryption cipher and gzip layers, native
MinimaDB stores, H2 RUNSCRIPT and the wallet key-use update. MiniData now has a
reader callback that shares length validation with its existing span decoder.
It reads exactly four header bytes and then exactly that section's payload,
preserving the point at which each database write becomes possible. A bulk
decompression would reject later damage before earlier writes had occurred.

Main.restoreReady(bool) remains its actual implementation and is now virtual,
matching the Java override boundary. The command invokes the real Main singleton.
Native resources use RAII, while database save flags and file effects follow the
source body; RAII does not introduce database rollback.

## Understanding the tests

The [restore lab](../../../../../../../../student-guide/new-tests/restore.md)
explains the fixtures before their tests. The
[Java oracle](../../../../../../../../support/RestoreOracle.java) and
[native fixture](../../../../../../../../support/restore.cpp) restore identical
backups into copies of a real H2 destination. Source and destination have distinct
preferences, histories, seeds and key uses. Synthetic fixture key rows isolate
the SQL update from key-generation cost. Both Java- and C++-generated backup files
are consumed. The [driver](../../../../../../../../tools/restore_differential.py)
checks exact saved files, in-memory stores, SQL rows, cached seed hash, close flags,
Main events and save-state permission after every case.

Normal and ASAN/UBSAN comparisons each pass 128 cases: 16 successful restores and 49
failures with saving disabled. Valid cases must independently replace target key
0x03/use999 with source keys 0x01/use261 and 0x02/use356, restore six relevant
transactions, clear uninstalled MiniDapp preferences and remove temporary files.
A source key with five previous uses becomes 261: restoration adds 256.

Tests truncate each frame, reject negative/oversized lengths, obstruct output
paths, damage cipher/gzip tails, and supply broken SQL/history sections. Some
bad trailers and extra plaintext are ignored because the command stops reading
after its sixth frame. This is source behavior, not an integrity guarantee.
Only direct EOF/NPE/class-cast diagnostic prose is normalized. Cipher errors
retain the Java cause class and message inside IOException.

The separate [actual-node test](../../../../../../../../tools/native_restore_startup.py)
creates all default keys through the real command, backs up one runtime, restores
using the other, and reopens the node. It exercises real worker shutdown rather
than the many-case fixture's controlled restoreReady override. Final evidence and
coverage limitations are recorded in
[validation](../../../../../../../../validation/restore-work-in-progress).

Both builds also pass six actual-node processes each, covering both restore
directions. All eight restored keys have 263 uses. A restored key signs successfully,
altered data fails verification, and only the signing key advances to 264 uses.

## Limits

Full file/API parity remains not_verified. The command has no rollback catch or
finally. shutdown:false still stops normal workers and restores state; it changes
final shutdown coordination. Memory allocation failure, arbitrary SQL extensions,
concurrent mutation and full public-network behavior remain uncertified. Fixture
SQL observations may reopen a connection only after closure/cache state has been
recorded. Persisted SQL seed and the pre-existing cached SeedRow are distinct
observations; the inherited RUNSCRIPT method does not explicitly refresh that cache.

## Check your understanding

Why is a failure in the final TxPoWList materially different from a failure while opening the GZIP header?

Pinned Java SHA-256: `a5ef120d4e88782a353c1f7663d2af471cab9c98e114a399cab21b3e99618043`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
