# Lab: restoring before archive synchronization

Written before the tests. Read all 515 lines of pinned restoresync.java. Reuse
restore's cipher/gzip and six-section readers, but preserve this command's distinct
ordering. After loading the wallet SQL it closes the wallet, closes history,
removes the archive, removes the temporary folder, and reopens through Main.
Only then does it inspect chain age and parse the key-use increment.

The valid-parameter list contains file/password/host, although runCommand reads
keyuses. Test direct calls and runner calls separately: a direct numeric keyuses
can work while the real runner rejects that parameter. A backup newer than two
days or lacking host takes the local finish path. That path increments all keys,
stops the processor, saves databases and clears the published Main instance.

Extend the existing RestoreOracle/restore.cpp fixtures. Reuse their real H2
source/target stores, malformed encrypted backups and file snapshots. Add a saved
tree tip with controlled timestamp so the age branch is actually reachable.
Exercise missing/invalid parameters, wrong passwords, truncated sections, malformed
SQL/history, empty trees and readiness failures. Check partial effects, key-use
counts, store replacement, source preferences and cleanup, not just success JSON.
Use an existing clock callback to bracket time-dependent assertions; do not replace
the production age comparison with an always-recent test shortcut.

Map performResync completely. It notifies first, parses host without a null check,
probes at -1, and returns an empty object if the probe fails. After success it
retains the cascade/tree while resetting archives/history connections, increments
or sets key uses, requests successive IBDs and processes them. Every fifth batch
checks used memory against 250 MiB before a full memory reset. Reuse the logger's
native allocator callbacks rather than inventing a fixed usage value. Preserve
the two bounded polling loops and final response; the caller performs shutdown.

Run real nodes for successful restore/reopen/signing too. Recent-backup tests must
show an invalid supplied host is ignored, and no-host restores must work regardless
of age. Remote synchronization, memory-threshold resets, cascades and multi-batch
processing need their own evidence. Do not claim the full command is verified from
its local finish path alone.

The first assertion reused restore's six-row history expectation. Both runtimes
returned six relevant rows plus the restored tree block. TxPowTree.readDataStream
calls addSQLTxPoW when reopening a saved tree. The corrected independent assertion
requires all six relevant rows and permits an additional non-relevant row only
when its ID equals the restored tip. The original failed observer report is saved;
no production change was needed for that discrepancy.

A later malformed-SQL case deletes the seed table. Reopening initializes a new
random seed, so exact hashes differed despite the same control flow. The fixture
now sets the existing GeneralParams.SEED_PHRASE/ANYSEED_PHRASE options after loading
the baseline. They affect only a newly initialized seed and are consumed by the
existing Wallet.initBaseSeed body. Production randomness is unchanged. This lets
the new-seed branch compare exact cached and SQL values; the initial report remains.

The null-host performResync case then found a production exception-category
mismatch: connect.createConnectMessage used optional.value(), exposing a C++
bad_optional_access as a generic RuntimeException. Java dereferences the host with
indexOf and throws NullPointerException. The shared native helper now performs an
explicit JavaNullPointerException check before parsing. Other parsing behavior is
unchanged; the existing network-startup corpus and new restoresync null-host cases
cover the affected boundary. The initial failure is retained.

Final differential comparisons pass all 181 cases in both builds. There are 39
success replies and 49 cases leaving save state disabled; a success reply alone
does not establish that malformed wallet SQL preserved the original keys. Six
actual node processes per build check both backup-writer/restore directions,
uses 7→263, restored-key signing to 264, changed-data rejection, original chain
and clean shutdown. Remote synchronization is not established by these local
recent-backup node tests.
