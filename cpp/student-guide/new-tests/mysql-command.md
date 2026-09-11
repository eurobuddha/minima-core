# MySQL archive command: preserve the order of effects

The Java source is `upstream-study/src/org/minima/system/commands/backup/mysql.java`.
Its C++ counterpart is `cpp/src/org/minima/system/commands/backup/mysql.cpp`.
This lesson accompanies the next command tests. Both the normal and sanitizer suites pass 2,800 comparisons, 2,061 independent
checks and 73,670 recorded SQL events per build. Their transcript hashes match.
This is evidence for the documented test scope, not proof of full node parity.

Think of this command as a coordinator. MySQLConnect owns the SQL statements,
ArchiveManager owns the local H2 archive, RawArchiveInput decodes raw archives,
and Main manages the running node. The port calls those existing implementations.
It translates the choices and their ordering rather than inventing a new database.

For example, saved login details are used unless both host and database parameters
are present. Providing just a host still uses the saved login. Providing both
requires user and password too. Tests compare the actual connection arguments,
returned JSON, and changed preferences, so a superficially correct reply cannot
hide a wrong database selection.

The recording tests run the unchanged pinned Java command and the native command
against equivalent SQL test drivers. Every statement, binding, query, and close
is recorded. Batch responses let a query return different data on successive calls;
this exercises real loops, including the final empty batch. This fixture is not
proof of real database persistence. Separate live database tests are still required.

Several source quirks matter. Integrity checks the parent hash before the block
number. Update uses a local archive query with exclusive bounds. Its condition
`lastarch != 0 || lastarch != 1` always holds. Raw export writes a block before
checking its record limit, so a zero limit can still write one block. Tests inspect
archive bytes and SQL events to make those differences visible. Fixmissing accepts
endfix but never reads it in the pinned source; adding a limit would change behavior.

Error injection interrupts each recorded SQL event in selected complete paths.
Comparisons check which writes occurred and whether shutdown was reached. Normal
exceptions during rawimport clear the Main import flag and become CommandException;
Java Error does not enter that catch. No test fixture adds a production loop limit.

For students: predict the connection arguments for every combination of saved login,
host, and database. Then trace a three-block archive across two batches. Finally,
compare a corrupt parent and a missing height to explain why the first reported
error depends on validation order. Keep teaching coverage separate from verified
runtime parity: resync, persistence, concurrency, and platform behavior need their
own evidence.

The first comparisons found two real adapter defects: missing files lost Java's
FileNotFoundException details, and truncated deflate streams lost their EOF
message. The corrected stream checks reject embedded NUL paths before opening,
use the existing MiniFile error format, and preserve the decompressor's EOF text
without changing which catch clauses recognize EOF. RawArchiveInput now routes
its messages through MinimaLogger. Earlier broad tests reduced errors to IO_ERROR,
which explains why these command-level differences needed a more precise test.

A separate fixture mismatch omitted the temporary H2 connection event. Another
captured demo logger output in a different channel. Those fixes change the test
harness, not production logic. The driver separates timestamped logger messages
and stack frames from the demo's plain console lines before comparison. Saved
initial failures distinguish implementation defects from harness defects.

The successful resync path, cascaded imports, multi-process database persistence,
long-running GC/interrupt timing and arbitrary platform behavior are still open.
In particular, calling SqlDB.restoreFromFile before wiping MySQL does not prove
restore succeeded: that dependency can catch and log some errors internally.

Both affected regression selections pass: 613 raw archive cases, 509 archive
command cases, 771 mysqlcoins cases, the SqlDB suite and ArchiveServer cleanup
audit. The complete 259-entry CTest suite has not been run.
