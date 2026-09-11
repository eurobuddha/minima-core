# Lab: serving archive blocks over a socket

The pinned ArchiveServer is a binary protocol server despite extending HTTPServer.
Its listener creates one handler per accepted socket. A handler reads one length-
prefixed MiniData, consumes a type byte without validating its value, then decodes
the requested first block. A request for -1 probes the database at zero and replies
with an empty IBD. A zero request includes the cached cascade when present; all
other requests include the loaded range. Returned null block entries are preserved.

We reuse the native HTTPServer listener, NIOConnections socket I/O, MiniData stream
reader, MiniNumber/IBD codecs and MySQLConnect. The handler's retry name is misleading:
it logs, sleeps one second, and calls loadBlockRange again; it does not construct a
new connection. SQL exceptions swallowed by MySQLConnect do not trigger this retry.
After transmitting the framed reply, the source sleeps ten seconds before closing.

Early returns matter. A failed outer MiniData read or MiniNumber read returns quietly,
without explicit stream closure or SystemClean. A missing type byte falls into the
outer logged exception path. A null range closes output and returns without cleaning.
Other ordinary failures are logged, followed by SystemClean. Java Error subclasses
escape those Exception catches. The native code follows the same catch boundaries.
Automatic C++ owner destruction is separate from these explicit close operations;
it cannot reproduce Java garbage-collector timing exactly.

SystemClean synchronizes its shared timestamp and runs only when elapsed time is
strictly greater than 10,000 milliseconds. It logs, requests collection, then stores
the new time. The native version uses the existing logger runtime clock/collection
callback and Java-width subtraction. No garbage collector is added to C++ ownership.

The main program loads the driver before environment or argument parsing. Arguments
override environment values; help deliberately prints v0.8 while startup logs v1.3.
EOF and empty input continue the source console loop; only trimmed quit ends it.
The listener uses the existing native shared-owner factory convention so its thread
cannot observe a partly constructed derived object. Missing-driver stack-frame text,
arbitrary MySQL subclasses and cross-platform socket behavior require explicit review.

Planned validation compares exact reply bytes, ignored type values, probe/cascade/
block ranges, malformed framing, retained sockets on early returns, actual ten-second
closure, one-second retry, constructor SQL effects, and console parameter behavior.
A real isolated MySQL fixture must also exercise the standalone native executable.
No completed ArchiveServer validation is claimed when this lab is introduced.

Both constructor/handler suites now pass 81 comparisons and 53 independent checks,
with the same transcript hash. Normal time94.016seconds; sanitizer94.269seconds.
The first run found a pre-existing IBD null-block exception mismatch; its source
NullPointerException is now preserved, and the failed report is retained.
The separate standalone test covers actual Java-written MySQL storage, TCP replies,
CLI parsing and process exit. Final counts are in the
[saved evidence](../../validation/archive-server-work-in-progress/progress.json).

The standalone suites pass in both builds: 19 CLI cases, two actual server
processes, four TCP requests per runtime and 110 independent assertions. Pinned
Java writes three blocks and a cascade to the disposable MySQL fixture. Both
servers read those bytes, return identical responses and close after ten seconds.
All four affected regression suites pass, including 1,057 IBD cases per build.

Review also caught a monitor detail: Java synchronized locks are reentrant. The
cleanup lock is now recursive. A separate four-program audit installs a Main log
receiver that calls SystemClean again once; both implementations complete and
emit two cleanup logs. All 11 assertions pass in each build. This audit does not
re-run the socket or timing cases; its evidence is labeled separately.
