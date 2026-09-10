# status — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/status.java) · [Mapped C++ body](../../../../../../../../src/org/minima/system/commands/base/status.cpp) · [C++ header](../../../../../../../../src/org/minima/system/commands/base/status.hpp)

## Purpose

status assembles a broad runtime report from chain, wallet, files, databases and network services. It reads them sequentially rather than under one snapshot.

## Follow the command

1. If clean is present, request System.gc regardless of its value. Read debug and complete, then fetch database/tree/cascade/wallet services and report version, uptime, locked state and MegaMMR mode.
2. complete additionally estimates devices as ceiling of block weight divided by a minimum-clamped user hashrate, multiplied by ten in low mining mode. This branch dereferences the tip before later no-tip handling.
3. For a present tip, combine branch length with cascade length, combine integer weights, and report MMR total and entry count. Otherwise emit zero values. Calculate RAM usage, recursively total data-directory size and report individual database file sizes; complete adds a three-level file listing.
4. For a tree root, report tip height/time/hash and chain statistics. Heights at most two use speed one; otherwise choose median-time endpoints and calculate chain speed, using minus one when endpoint block difference is zero. No-root output has fewer fields.
5. Attach cascade data, mempool/RAM/SQL/archive counts and optional network status. Debug logs mark each completed phase. The standalone main only prints a hardcoded directory-size experiment.

## Translation and dependencies

The native command calls the existing MinimaDB, Main, MiniFile, MiniFormat and
TxPoWGenerator implementations. Its DECIMAL32 division reuses MMREntryNumber's
seven-digit HALF_EVEN calculation, then passes through the actual MiniNumber
constructor, division and ceiling. MiniData.withMinLength pads the displayed
difficulty without truncating longer input. JSON raw decimal values preserve the
source's numeric speed field; block heights narrow to Java long while total
length and branch subtraction preserve Java int wraparound.

The existing MinimaLogger Runtime supplies allocator memory statistics and an
optional collection-hint callback. Main binds its existing collectMemory service
to this boundary. The default hint does nothing: native ownership replaces JVM
collection. Calling it before any Main lookup preserves clean-key handling even
when the Main singleton is absent. No lower RAM usage is promised.

## Understanding the tests

The [status lab](../../../../../../../../student-guide/new-tests/node-status-command.md)
was written before the tests. The [corpus](../../../../../../../../tools/node_status_commands_corpus.py)
uses actual command registration, database children, synthetic chain nodes and
controlled SQL rows/files. It compares metadata, direct typed parameters, runner
permissions, optional network output, empty tips, SQL failures and debug order.
Independent assertions check device estimates, median speed, file totals, wallet
lock state, response types and absence of unintended queue/timer effects.

Two initial expectations were corrected using the source algorithm: Python's
scientific integer notation must become a plain integer string, and timestamps
0, 0, 1000, 2000 have median block 2, giving two blocks per second against block 0.
The production arithmetic did not change to satisfy those mistaken expectations.

Each runtime's uptime is checked against surrounding timestamps before comparison.
Native controlled heap bytes are asserted exactly, including signed wraparound;
Java's actual heap display is validated for shape and bounds before excluding
cross-runtime size equality. A native-only counter proves collection-hint presence
and order. That counter does not prove JVM collection. The standalone static main
is also invoked and its exact printed output compared.

## Limits

The report may mix states observed at different times and recursive disk inspection can be expensive. clean requests Java collection but guarantees neither collection nor a particular RAM reduction.

## Check your understanding

Why can status complete:true fail on a no-tip node even though normal status has explicit zero-value output?

Pinned Java SHA-256: `8d0247e53440969ace576d46015982e7dfb75c86825160e3e7877f55eab4e0eb`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
