# ArchiveServer — teaching guide

**Teaching status:** Authored explanation of the Java source, required translation, and proposed validation. The native counterpart is absent.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../upstream-study/src/org/minima/utils/mysql/ArchiveServer.java) · `src/org/minima/utils/mysql/ArchiveServer.cpp` (absent)

## Purpose

ArchiveServer serves binary Minima archive requests over accepted sockets while reusing HTTPServer’s listener infrastructure. Its handler does not parse HTTP requests; it reads length-prefixed MiniData and returns an archive IBD message.

## Follow the algorithm

1. Construction creates/initializes MySQLConnect, loads a cascade once, logs its presence, and starts the listener. Each accepted socket receives an ArchiveHandler.
2. The handler reads a complete outer MiniData, then a message-type byte and first-block MiniNumber. The type is read but not validated. Failure reading the outer envelope or block number returns early, treating input as unrelated traffic.
3. First block -1 probes the database by loading from zero but sends an empty IBD. Otherwise block zero includes the cached cascade when available, and the loaded ascending block range is appended. The reconnect helper catches an escaping Exception, sleeps one second, and retries once; it does not itself reopen the connection.
4. Create MSG_ARCHIVE_DATA bytes using NIOManager, write them as MiniData, flush, wait ten seconds, then close streams. Outer exceptions log. Early returns bypass some closing and the final SystemClean call because cleanup is not in a finally clause.
5. SystemClean is synchronized/static and logs/requests GC only when more than ten seconds elapsed. main loads the JDBC driver, takes environment defaults then command-line overrides, starts the server, and reads stdin until quit. EOF does not exit; unknown options/help exit with status one.

## Translation work still required

The mapped native counterpart is absent. HTTPServer socket lifecycle, MiniData framing, MiniNumber/IBD/Cascade serialization, NIO message construction and MySQLConnect are all required dependencies. A generic HTTP JSON endpoint would change the protocol. Native GC/interrupt/sleep/resource ownership need explicit adapters and documented gaps.

## Understanding the tests

There is no dedicated automated native ArchiveServer fixture claimed. Proposed loopback tests should send exact binary envelopes for -1, zero and a later block, verify the response type and decoded IBD/cascade, then exercise malformed input, unvalidated type bytes, null/partial database ranges, retry timing and early-close behavior. A recording MySQL fixture can verify calls; actual archive persistence needs a separate database test.

- [Actual range-loading and SQL exception behavior](../../../../../../../../upstream-study/src/org/minima/utils/mysql/MySQLConnect.java)

This lesson is based on the pinned Java source. Proposed native tests are future work; no native implementation, build, or test pass is claimed.

## Limits and a useful next test

No listener or database was started for this guide. The retry wrapper may not retry ordinary SQL failures because MySQLConnect.loadBlockRange catches SQLException and returns a partial/empty list. Tests must distinguish an escaping exception from a swallowed one.

## Check your understanding

Why can a method named reconnectLoadTxBlocks return an empty response without reconnecting after a database error? Which early returns skip SystemClean?

Pinned Java SHA-256: `3b2918c85d07ee57035e4b5a2d0f7033cdf78706f426f973075d338aea34dfa5`. [Source mapping](../../../../../../../source-map.json). [Project-wide limitations](../../../../../../../REVIEW.md).
