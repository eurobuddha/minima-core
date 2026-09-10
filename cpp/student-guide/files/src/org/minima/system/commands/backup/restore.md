# restore — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/restore.java) · `src/org/minima/system/commands/backup/restore.cpp` (absent)

## Purpose

restore replaces wallet and node databases from the six-section encrypted backup and normally stops the node for restart.

## Follow the command

1. Require all keys created and an existing file, read default password and shutdown switch. Load the whole backup, decode salt/IV, derive its cipher and open GZIP; only GZIP construction failures are relabeled Incorrect Password.
2. Call restoreReady(shutdown) before consuming all sections. Write wallet SQL to a temporary folder, disable state saving, and overwrite cascade, chain, user preferences and P2P files directly as each MiniData section is read.
3. Load those databases, clear uninstalled MiniDapp preferences, decode the relevant TxPoWList, wipe transaction SQL and insert each TxPoW as relevant. Close streams and re-enable state saving.
4. Restore wallet SQL and increment every key-use count by 256. Close transaction SQL, close/delete the archive database file, remove the temporary folder and save all databases.
5. Return restore.file and a restart message. When shutdown is true, mark shutdown complete, stop Main’s processor and notify its shutdown listener. There is no enclosing finally/rollback for section failure.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse the exact inverse of backup framing, database load/restore helpers and key-use updates. C++ resource management must not be confused with all-or-nothing database restoration, which the Java body does not provide.

## Understanding the tests

The runner fixture checks restore classification only. Proposed isolated tests restore Java fixture sections, check exact write/load order and key-use increment, and inject corruption at every section. Inspect save-state flag, temporary files, already-overwritten databases and shutdown callbacks after failures, including truncated final TxPoWList.

- [Runner fixture using selected fake commands](../../../../../../../../support/CommandRunnerOracle.java)
- [Native runner fixture](../../../../../../../../support/command_runner.cpp)
- [Runner-only dispatch/permission coverage](../../../../../../../../tools/command_runner_differential.py)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

No live state was restored. A valid GZIP header does not validate the remainder before mutation begins. shutdown:false still performs the destructive restoration and archive reset; it only changes shutdown coordination.

## Check your understanding

Why is a failure in the final TxPoWList materially different from a failure while opening the GZIP header?

Pinned Java SHA-256: `a5ef120d4e88782a353c1f7663d2af471cab9c98e114a399cab21b3e99618043`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
