# Lab: deriving a coin index from archived blocks

This plan precedes the command comparison. The native mysqlcoins class reuses
MySQLConnect, UserDB, Command, the existing block/coin codecs and logger date
format. The SQL fixture will record the real connection class's statements and
bindings, just as in the existing MySQLConnect tests. It is not a live database.

The command has five actions. Info counts coins and reads the latest creation
block. Wipe drops and recreates the coin table and deliberately returns the pinned
misspelling `respnse`. Autobackup changes a preference and either stores or clears
credentials; it does not change the separate auto-login flag. Update reads blocks
and inserts their input and output coins. Search builds or accepts SQL.

Update marks input coins spent and binds their spending block. It marks output
coins unspent, changes their creation block to the current block, and binds zero
as their spending block. The coin limit is checked after a whole block and uses
strictly greater-than, so a limit can be exceeded. The block limit is checked
before processing each block. Keep these two checks separate when predicting
the final counts. The source increments coin counts even if insertCoin catches
and logs a SQL exception.

Search gives `where` priority over `address`, then `query`. Only the address form
adds spent, ordering and limit clauses. Auto-login credentials override explicit
parameters. Mobile rejection happens before parameter parsing or SQL connection.
Shutdown is explicitly reached only after a successful command path; moving it
into an unconditional cleanup block would change observable source behavior.

The tests will compare metadata, parameter errors, preferences, all five actions,
exact SQL events and bindings, and exception handling. Recorded block rows are
synthetic valid serialized blocks from the existing fixtures. Repeated ranges
will be bounded with maxblocks; these tests do not establish real SQL pagination,
durability or concurrent archive behavior. A separate live MySQL driver remains
necessary for complete runtime parity. Integer increments use Java-width wrapping
in native code; extreme counter iteration counts are not feasible test fixtures.

Both complete builds and both comparison suites pass: 771 cases, 1,007 independent
assertions and 24,372 SQL events per run. Normal time14.523seconds; sanitizer time
25.142seconds. Transcript hashes match, and native stderr is empty. The initial
normal run exposed an exception-labeling mistake in the fixture, fixed using the
existing typed CommandException adapter. The failed report remains in evidence.
