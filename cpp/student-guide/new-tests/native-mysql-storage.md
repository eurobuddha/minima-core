# Lab: connecting the C++ node to a real MySQL server

The mysqlcoins command comparison used recording SQL. Those tests established
which calls the command made, but could not store a real row. This lab addresses
that missing runtime boundary. The application remains C++; the existing native
H2 adapter is reused to compile the pinned third-party MySQL JDBC driver into the
dependency library. No Java Minima classes are linked into that library, and the
native executable must not launch a JVM to execute its database calls.

The repository already supplies Connector/J 8.0.24. Its SHA-256 is checked before
building. Reusing this exact driver keeps its protocol, prepared-parameter binding,
type conversions and SQL exceptions. Translating those behaviors again in an
unrelated connector would create a second compatibility task. The C++ adapter's
existing packet format already supports nullable strings, binary bytes, doubles,
metadata and object ownership; it only needs to expose nullable login parameters.

Java libraries can load classes and resources dynamically. Ahead-of-time builds
need metadata describing those accesses. The first phase runs the pinned driver
on the JVM against a disposable localhost MySQL container to collect that metadata.
The JVM is a build/validation tool here. The final native library is tested separately.
Generated metadata is reviewed to exclude Minima application classes and the test
oracle. Follow the [GraalVM tracing-agent documentation](https://www.graalvm.org/latest/reference-manual/native-image/metadata/AutomaticMetadataCollection/).

The existing NativeH2Oracle and C++ SQL fixture execute the same operations with
MySQL connection URLs. Cases include prepared parameters, null and empty strings,
Unicode and embedded zero bytes, binary blobs, numeric bounds, SQL errors, metadata,
transactions and rejected credentials. Java and C++ also read each other's committed
rows on the same real server. Native handle counts must return to zero after each
program, so result objects do not silently keep database owners alive forever.

The native H2 tests and actual node startup tests must also pass after the shared
adapter changes. A driver test alone does not prove the mysqlcoins command is wired
into the real node. Native Minima runtime integration and remaining command files
stay part of the full project goal.

The final metadata profile passes 52 programs. Three earlier fixture/setup failures
are retained: an unsupported class filter, Java26 bytecode loaded by GraalVM25,
and an assertion that expected the inner authentication message rather than the
source autoReconnect wrapper. None was corrected by changing driver behavior.

Both complete C++ builds pass with the new native library. Each live driver suite
passes 52 programs, 444 operations and 169 independent checks. The Java/C++ transcript
hashes match across normal and sanitizer builds, and native stderr is empty.
Existing H2 differential and concurrent tests also pass in both builds.

The node test reuses Node, ready and key_state from native_restore_startup.py,
and the shutdown stderr capture from native_base_test_startup.py. It launches
actual solo nodes, creates all eight default keys, runs mysqlcoins through their
real command runners, and restarts them. It checks info, wipe, empty-archive update,
SQL insertion, persistent rows, six-place double output, Unicode, null omission,
token hiding, errors and read-only connection setup. Both runtimes must return the
same replies; only update duration is checked by shape then removed for comparison.
The SQL insert is deliberately issued through the source's query action. It tests
real storage independently of block decoding. Nonempty archive-to-coins updates,
autologin/autobackup integration, reconnect faults and broader TLS/platform behavior
still require additional tests. Full API and node parity remain open.

See the [saved evidence](../../validation/native-mysql-work-in-progress/progress.json)
for final node results and [build instructions](../../native/h2/README.md) for
reproducing the optional driver and isolated fixture tests.

Both actual-node runs passed: four processes and 17 mysqlcoins commands per
runtime in each build. Normal duration22.666seconds; sanitizer134.169seconds.
All eight wallet keys survived restart, rows remained queryable, and shutdown
diagnostics contained no sanitizer failures.
