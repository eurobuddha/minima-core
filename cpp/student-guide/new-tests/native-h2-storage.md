# Lab: preserving the database engine across the language boundary

Written before the native H2 experiment. SqlDB uses H2 MySQL mode, MVStore files,
AES-encrypted files, SCRIPT/RUNSCRIPT backups and JDBC result conversions. The
sibling SQLite Database wrapper cannot preserve those behaviors. Its file format,
encryption and SQL administration commands differ from this pinned source.

The experiment compiles the existing H2 2.4.240 dependency into a native shared
library with GraalVM. Minima code remains C++; a small bridge calls JDBC methods
without rewriting their SQL or file handling. The bridge is a dependency adapter,
not a substitute Java Minima application. No JVM fallback is permitted in this
build. The existing SqlConnection interface supplies the native callers.

Test real persistence, cross-opening files between JVM H2 and the native library,
encryption/password failures, schema constraints, prepared bindings, nulls,
metadata, object conversions, close/reopen, script backup/restore and concurrent
connections. Compare the pinned H2 engine's actual results and exception classes.
Then connect Main and prove private-node startup, command execution and shutdown.
Record what passes and what remains before declaring the provider integrated.

Reference: [H2 native-image build support](https://h2database.com/html/build.html#native_image)
and [GraalVM C shared-library interface](https://www.graalvm.org/dev/reference-manual/native-image/guides/build-native-shared-library/).
H2 calls its native-image support experimental; user functions, third-party
extensions and arbitrary runtime-loaded Java classes need separate validation.


## Follow one call

A prepared INSERT first creates a JDBC statement handle. C++ encodes each binding
with an operation number, parameter index and value. Strings are UTF-16 code units;
null has length -1 and empty has length 0. A native thread attaches to the H2
isolate, makes the C ABI call, copies its response, and detaches. The bridge catches
Java exceptions before the C boundary and returns their type and message as data.
C++ reconstructs the corresponding exception category.

Each child handle retains its parent. A result can therefore outlive a local
statement variable without losing its connection. Dropping a C++ result view must
not close a JDBC result still retained by its statement: getResultSet can return
that result again. Explicit close still calls the real JDBC method.

## Tests and what they establish

The real-engine comparison runs 122 programs containing 1,974 operations. It
covers null/empty/large bindings, Unicode and embedded NULs, numeric limits,
metadata, error classes, prepared overloads, closed statements, retained results,
CLOB ranges, commit/rollback and actual SqlDB SCRIPT/RUNSCRIPT backups. Four
additional cases cross-open plain/AES files between Java and native H2 in both
writer directions, including wrong-password rejection. Results and exceptions
are compared without normalization. The native handle registry must return to
zero after each program.

The concurrent fixture follows the existing sql_db_concurrent.cpp barrier/join
pattern, now against real H2. Eight workers insert 64 disjoint IDs each. Count 512
and sum 130816 detect lost or duplicate rows. Each worker drops parent variables
before reading a child result. A separate connector survives its Runtime owner.
These assertions test lifetime and concurrency, not merely successful compilation.

The startup comparison runs twelve real processes: Java → C++ → Java and
C++ → Java → C++, each with plain and encrypted wallets. A fresh solo node creates
genesis, answers hash/status, and saves on quit. Its next two processes reopen the
same directory; the last compacts its databases. Exact stable chain fields must
survive, and the known SHA3 response must match. Uptime, memory, timestamps and log
ordering are intentionally outside this observation. The nodes do not connect to
public peers. Long-running mining, network sync, RPC/daemon and recovery remain
separate work.

## The initial failed test was an observer bug

The empty-BLOB case failed with basic_string. The bytes had crossed the database
boundary correctly. The fixture called to0xString().substr(2), but MiniData returns
an empty string for empty bytes; there are no two prefix characters to remove.
Calling the existing toHexString(false) expresses the desired observation directly.
The saved initial failure prevents us from rewriting history as an immediate pass.

Ask yourself: why would closing every object in a C++ destructor break JDBC result
ownership? Why are real encrypted file reopen tests stronger storage evidence than
recording an SQL string that says CIPHER=AES?

Build instructions: [native dependency README](../../native/h2/README.md).
Evidence: [native H2 checkpoint](../../validation/native-h2-work-in-progress).
