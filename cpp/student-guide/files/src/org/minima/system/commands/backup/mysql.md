# mysql — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/mysql.java) · `src/org/minima/system/commands/backup/mysql.cpp` (absent)

## Purpose

mysql connects archive persistence, chain resynchronization and several import/export formats to MySQLConnect. Many actions mutate local settings or chain state as well as the remote database.

## Follow the command

1. Reject mobile mode and read action default info. Saved credentials are used unless both host and database are supplied, in which case all four fresh credentials are required. Initialize the connector before dispatch, including setlogin/clearlogin. info reports endpoint differences as totals (without +1), masks the password and can dereference a missing endpoint when only the other archive endpoint exists.
2. setlogin and clearlogin edit credential fields and autologin; autobackup edits its flag and stores/clears credentials, without explicitly saving UserDB here. wipe delegates wipeAll, while size asks for row count. These are distinct from info’s height-difference totals.
3. integrity scans ascending MySQL ranges and rejects a wrong parent hash before checking sequential height. update requires local archive blocks, selects a starting height from newest MySQL data and copies ascending local ranges with adjacency checks among newly copied blocks. The empty-database condition lastarch != 0 OR lastarch != 1 is always true, selecting lastarch as the exclusive local range start and potentially skipping that oldest block.
4. resync requires completed keys, notifies MiniDapps and cleans the phrase even when absent. A supplied phrase resets wallet seed/keys/use counters; otherwise retain keys during chain reset. Clear MegaMMR, read MySQL batches from zero and post IBDs without the commented-out cascade installation. Poll for a tip and exact end height with limits; error breaks still lead to a completed response and shutdown.
5. addresscheck scans MySQL TxBlocks ascending and records matching output/input events with block ID/date/coin JSON under coins. Optional statecheck converts Mx values unless they contain @; it does not additionally scan the current retained chain.
6. h2import restores an H2 temporary archive before wiping MySQL, copies its cascade and blocks, then closes/deletes temporary data. h2export copies MySQL blocks into an H2 temporary archive, adds a cascade when available and writes compressed SQL. Neither workflow is one transaction across all records and files.
7. rawexport writes GZIP containing cascade flag/optional cascade, declared MiniNumber row count (capped by maxexport), then TxBlock bytes. As in archive export, limit checking follows the first write, allowing a zero limit/header to coexist with a block. rawimport sets MYSQL_IMPORTING_NO_ACTION, connects the raw reader, wipes MySQL, saves cascade/blocks and clears the flag on normal or caught-exception exit; earlier saved records are not rolled back.
8. reset runs nested mysql rawimport then resync and checks each status, but forwards neither supplied login details nor phrase/key options in those nested command strings. fixmissing scans a raw file and saves absent heights strictly greater than startfix; endfix is accepted but unused and existing wrong-content blocks are left untouched.
9. findtxpow gives txpowid precedence over block. Search the separate TxPoW table then block-by-ID fallback, or block-by-number, and return found plus optional JSON. Invalid actions throw. The common mysql.shutdown runs only on normal completion, not in a finally.
10. convertMySQLParams splits username:password@host/database using first delimiters, sets global/store/autobackup preferences, optionally delays, then tests a readonly connection. It catches/logs errors without reverting settings already changed. The standalone main merely connects to hardcoded local example credentials and prints row count; it is not an assertion suite.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse pinned MySQLConnect, ArchiveManager, RawArchiveInput and TxPoW processing with their exact inclusive/exclusive range semantics. In MySQLConnect, loadFirstBlock means highest and loadLastBlock means lowest; relying on everyday meanings reverses traversal.

## Understanding the tests

The runner fixture checks mysql classification only. Proposed direct fixtures assert credential override rules, height-difference versus count, update’s oldest-block boundary, mismatch check order, connection cleanup and nested reset arguments. Format tests compare complete raw bytes and H2 export contents in disposable stores. Failure tests inspect import flags, partial remote rows/settings and sync timeout responses. No live database or credentials are needed for deterministic orchestration tests.

- [Runner fixture using selected fake commands](../../../../../../../../support/CommandRunnerOracle.java)
- [Native runner fixture](../../../../../../../../support/command_runner.cpp)
- [Runner-only dispatch/permission coverage](../../../../../../../../tools/command_runner_differential.py)
- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

No MySQL connection was made. Connector methods can catch SQL errors internally, so a completed copy loop does not necessarily prove all rows were stored. readonly is passed to the helper and should not be presumed to enforce a database-wide access restriction. Full operational recovery needs integration evidence beyond source explanation.

## Check your understanding

Why is lastarch != 0 || lastarch != 1 true for every value, and what happens when the archive range helper excludes its lower bound?

Pinned Java SHA-256: `1cf8f858f0329c43d139ebeca7de58876b036010afaa95e696a1077cde930265`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
