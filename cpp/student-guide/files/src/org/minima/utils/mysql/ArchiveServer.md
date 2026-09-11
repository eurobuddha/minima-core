# ArchiveServer — teaching guide

**Teaching status:** Authored explanation of the source, native translation, tests and remaining limits.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/mysql/ArchiveServer.java) · [Native counterpart](../../../../../../../src/org/minima/utils/mysql/ArchiveServer.cpp)

## Purpose

ArchiveServer serves binary Minima archive requests over accepted sockets while reusing HTTPServer’s listener infrastructure. Its handler does not parse HTTP requests; it reads length-prefixed MiniData and returns an archive IBD message.

## Follow the algorithm

1. Construction creates/initializes MySQLConnect, loads a cascade once, logs its presence, and starts the listener. Each accepted socket receives an ArchiveHandler.
2. The handler reads a complete outer MiniData, then a message-type byte and first-block MiniNumber. The type is read but not validated. Failure reading the outer envelope or block number returns early, treating input as unrelated traffic.
3. First block -1 probes the database by loading from zero but sends an empty IBD. Otherwise block zero includes the cached cascade when available, and the loaded ascending block range is appended. The reconnect helper catches an escaping Exception, sleeps one second, and retries once; it does not itself reopen the connection.
4. Create MSG_ARCHIVE_DATA bytes using NIOManager, write them as MiniData, flush, wait ten seconds, then close streams. Outer exceptions log. Early returns bypass some closing and the final SystemClean call because cleanup is not in a finally clause.
5. SystemClean is synchronized/static and logs/requests GC only when more than ten seconds elapsed. main loads the JDBC driver, takes environment defaults then command-line overrides, starts the server, and reads stdin until quit. EOF does not exit; unknown options/help exit with status one.

## Translation and ownership

The C++ class reuses HTTPServer's shared-owner listener factory, the actual
MySQLConnect methods, MiniData::readFromStream, native socket reads/writes and the
IBD/NIO codecs. create performs the source constructor's SQL initialization and
cascade lookup before starting its thread. The handler retains its server and
socket while it executes. Explicit close operations follow the source branches;
automatic C++ destruction cannot reproduce Java garbage-collector timing.

The main entry point has its own minima_archive_server executable. The optional
native database provider adds an explicit driver-load operation before parsing.
It contains the pinned third-party JDBC implementation, no Java Minima application
classes and no JVM fallback. ConsoleInput, Java integer parsing and trimming are
existing native adapters. Help retains the source v0.8/v1.3 inconsistency and EOF
retains the source's continuing console loop.

## Understanding the tests

The [lab](../../../../../../../student-guide/new-tests/archive-server.md) precedes
execution. Both complete builds and the 81-case constructor/handler/retry/clean
comparison pass, with 53 independent checks. The tests run actual source methods,
real socket I/O, the existing recording SQL driver and unchanged codecs. They
compare SQL events, exceptions, logs, exact framed bytes and explicit socket state.
All four successful reply cases retain the real ten-second source wait. Runtime
range failures exercise the one-second retry; SQL failures caught by MySQLConnect
do not trigger it. Malformed envelopes and number encodings return quietly while
the accepted socket is still retained; a missing type byte logs and cleans instead.

The final malformed stored-block case exposed an existing IBD serialization error:
Java dereferences a null TxBlock and throws NullPointerException, while native code
threw IllegalArgumentException. The serializer now checks that specific null entry;
both comparisons pass and the original failure remains saved.

A separate actual-main test uses the real pinned MySQL driver and a disposable
localhost database. Pinned Java writes three blocks and a cascade; actual Java and
native ArchiveServer processes read them through MySQL, serve exact TCP replies,
wait ten seconds before closing and respond to console quit. CLI cases verify help,
missing arguments, integer parsing, environment precedence and exit statuses.
See [current evidence](../../../../../../../validation/archive-server-work-in-progress/progress.json)
for final results, including the sanitizer run and affected regressions.

## Limits and a useful next test

Parity remains unverified. Enhanced NPE/class-cast/EOF prose and stack frames are
normalized in comparisons. Exact missing-driver stack-frame text, arbitrary MySQL
subclasses/public-field races, interruption delivery, GC timing, resource exhaustion
and other platforms are not certified. The native range collection cannot represent
all Java subclass/alias behaviors. Source null-range handler behavior is covered
through a handler subclass, not a substituted production database.

## Check your understanding

Why can a method named reconnectLoadTxBlocks return an empty response without reconnecting after a database error? Which early returns skip SystemClean?

Pinned Java SHA-256: `3b2918c85d07ee57035e4b5a2d0f7033cdf78706f426f973075d338aea34dfa5`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).

## Runtime sleep follow-up

The default sleep now uses NativeThread. Ten new real-interrupt retry comparisons show that escaping runtime errors and null block numbers reach the retry pause and consume interruption; SQLExceptions caught inside MySQLConnect leave it pending. The retry still occurs once after interruption. The separate ten-second post-response interrupt path is not newly validated. See the [runtime sleep lab](../../../../../../new-tests/runtime-sleep-interruption.md) for the experiment, results and limits. Full parity remains not_verified.
