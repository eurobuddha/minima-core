# MinimaDB — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java) · [Native body](../../../../../../src/org/minima/database/MinimaDB.cpp)

## Node-state command comparison

The chain-service getAllCoins bridge preserves nullable SQL Coin results through the existing native Coin handle. Java adds null to the list without dereferencing it. Eager native dereferencing skipped a later lookup in cointrack; preserving the handle restores the observed lookup and failure order. A null handle still throws when a consumer reads its fields. See [the lab](../../../../../new-tests/node-state-commands.md) for the failing examples and final comparison evidence.

## Purpose

MinimaDB owns the node's database objects and coordinates their startup, saves, replacement and shutdown. It is the place to study how a system combines SQL databases with serialized state files while exposing a shared current instance.

## Follow the logic

1. createDB replaces the static singleton with a new owner. Construction creates archive, TxPoW storage, tree, cascade, wallet, user/P2P stores and MegaMMR, but leaves the custom TxnDB null until loadAllDB creates it. Getters expose those same objects. Replacing tree/cascade, or the singleton itself, does not mutate an old object retained by a caller.
2. readLock and writeLock use a reentrant read/write lock unless DB_IGNORE_LOCKS is set. Failed read unlock is logged; failed write unlock escapes. Acquiring a write lock records the current thread name; the public boolean records the last requested write action, not the actual recursive hold count. safeReleaseReadWriteLock releases at most one write hold and one read hold owned by the current thread, and does not update that diagnostic boolean.
3. loadAllDB holds the write lock and loads the wallet first, optionally through encrypted SQL. Archive or TxPoW SQL load failures each trigger logging, hard close, deletion of that database directory, object replacement and one reload. A failure in that recovery reaches the outer error handler. Wallet startup has no equivalent local wipe/retry branch.
4. Next it loads user preferences, creates/loads TxnDB, deletes the old coin database directory and creates CoinDB, then loads cascade, chain tree and both P2P stores. It validates the cascade, loads MegaMMR when enabled or deletes its saved file otherwise, and requires tree root height to equal cascade tip height plus one when both exist. In that same nonempty branch, a nonempty MegaMMR must end one height before the tree root.
5. After successful loading, ArchiveManager decides whether to store the cascade. On outer failure, a configured rescue node gets a timer for MAIN_DO_RESCUE after 1000 ms; otherwise mobile/JNLP gets a startup-error callback, while desktop halts with code zero. The final unlock is after the catch rather than in finally, so a throwing failure callback can leave the lock held.
6. loadArchiveAndTxPoWDB replaces archive and TxPoW owners and optionally wallet. loadDBsForRestoreSync also replaces wallet, cascade and tree before loading them. They log caught exceptions and retain whichever replacements already occurred. fullDBRestartMemFree clears RAM, requests SQL cleanup, saves SQL owners, then replaces/reloads wallet, archive and TxPoW storage; it catches SQLException only, so an unexpected runtime failure can skip its trailing unlock.
7. saveAllDB calls saveState then saveSQL and logs completion even when a child path swallowed a failure. saveSQL holds a write lock, always requests wallet compaction, uses the caller's compaction flag for TxPoW/archive and closes CoinDB without compaction; its finally releases the lock. A failure stops later statements inside that try.
8. saveState returns immediately when saving is disabled; otherwise it takes a read lock and saves custom transactions, user/P2P stores, cascade, tree and optionally MegaMMR in order. Before startup, null TxnDB fails at the first step and later saves are skipped. saveUserDB uses the same gate and a read lock. saveP2PDB uses the gate but has its lock calls commented out. These are sequential file writes, not one transaction across all stores.
9. getBaseDBFolder appends databases to DATA_FOLDER; file getters and size helpers derive individual paths, reporting zero for missing files. Coin notifications form a set separate from wallet tracking: duplicate adds collapse, removal reports whether membership existed, and membership does not require a stored script.

## Translation and dependencies

The native owner reuses the mapped child classes. shared_ptr preserves retained-object identity when fields are replaced; small mutexes protect owner access and notification membership, and atomic flags hold exposed state. A custom reentrant lock preserves the observed recursive behavior. Java-compatible path helpers preserve Unix File joining, and notification keys use UTF-16 semantics including null. MinimaDBServices requires SQL and Main callbacks explicitly; bindRuntime connects MMR, tree, search, IBD and consensus service lookups to the current singleton. These adapters are needed because the complete Main runtime and a native H2 engine are not available through this owner alone. Java System.gc has no corresponding collection call in C++.

## Understanding the tests

The complete MinimaDBOracle runs the pinned owner and actual child implementations, using recording JDBC and a Main fixture allocated without starting its constructor/worker. The native fixture supplies matching external callbacks. Identity arrays verify which owners changed and which held references survived; operation output includes lock counts, diagnostics, user data, SQL events, callbacks and file sizes. Cases cover null/empty/Unicode paths, notification membership, zero through three recursive read/write holds and safe releases, save gates, encryption/debug flags and retained TxnDB references across singleton replacement. Each of seven lifecycle commands sweeps failure positions 1 through 150 with checked and runtime failures. Additional cases cover null SQL boundaries, rescue timers, global versus held MMR lookup, nonempty tree/cascade/MegaMMR restoration, boundary-height mismatches and four files truncated to twelve chosen lengths. The driver runs each side in the same private temporary directory after clearing its previous fixture contents, then compares all remaining file bytes. Timer deadlines and SQL insert/cleanup timestamps are range-checked before normalization, rather than simply discarded.

- [Actual Java owner and callback harness](../../../../../../support/MinimaDBOracle.java)
- [Native owner harness](../../../../../../support/minima_db.cpp)
- [Lifecycle cases and file-byte comparison](../../../../../../tools/minima_db_differential.py)
- [Recording JDBC boundary](../../../../../../support/SqlDBOracle.java)
- [Native recording SQL boundary](../../../../../../support/SqlRecordingFixture.hpp)
- [Native owner and required runtime services](../../../../../../src/org/minima/database/MinimaDB.hpp)

No build or test was run while writing this lesson. The inspected driver defines the evidence scope; it does not certify full parity.

The [transaction-check lab](../../../../../../student-guide/new-tests/transaction-check.md) now calls the actual generator/checker through services installed by bindRuntime. Those callbacks resolve the current singleton’s real tree, mempool and user preferences. This extends runtime ownership wiring; Main startup and the native SQL engine remain separate unfinished work.

## Limits

Recorded SQL establishes call order, parameters and failure handling, not H2 persistence or encryption. Actual temporary state-file bytes provide stronger serialization evidence, but do not establish crash consistency across those files. Main messages/startup errors are recorded, notification callbacks are empty in this fixture, and desktop halt is outside the mobile-configured corpus. Recursive-lock cases are single-threaded; full runtime integration and contention remain unverified. Lock identity strings, logger details and enhanced null-pointer expressions are normalized.

## Check your understanding

Why can getCustomTxnDB return null immediately after construction? If a thread takes two write locks and calls safeReleaseReadWriteLock once, what remains held and why can the diagnostic boolean be misleading?

Pinned Java SHA-256: `2df5a41b570b02bd553f4c9c7c8db352c276b2653e1a92a38d9c54b1a2ed9d67`. [Mapping](../../../../../../source-map.json) · [Project limitations](../../../../../../REVIEW.md).
