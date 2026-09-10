# status — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/status.java) · `src/org/minima/system/commands/base/status.cpp` (absent)

## Purpose

status assembles a broad runtime report from chain, wallet, files, databases and network services. It reads them sequentially rather than under one snapshot.

## Follow the command

1. If clean is present, request System.gc regardless of its value. Read debug and complete, then fetch database/tree/cascade/wallet services and report version, uptime, locked state and MegaMMR mode.
2. complete additionally estimates devices as ceiling of block weight divided by a minimum-clamped user hashrate, multiplied by ten in low mining mode. This branch dereferences the tip before later no-tip handling.
3. For a present tip, combine branch length with cascade length, combine integer weights, and report MMR total and entry count. Otherwise emit zero values. Calculate RAM usage, recursively total data-directory size and report individual database file sizes; complete adds a three-level file listing.
4. For a tree root, report tip height/time/hash and chain statistics. Heights at most two use speed one; otherwise choose median-time endpoints and calculate chain speed, using minus one when endpoint block difference is zero. No-root output has fewer fields.
5. Attach cascade data, mempool/RAM/SQL/archive counts and optional network status. Debug logs mark each completed phase. The standalone main only prints a hardcoded directory-size experiment.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse formatting, median-time/chain-speed and filesystem helpers. Native process-memory and garbage-collection semantics require an explicit adaptation; C++ has no Java System.gc equivalent.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed tests use fixed services, clock, filesystem and memory callbacks, then assert field types and phase order. Cover clean:false presence, no tip with complete false/true, root-tip inconsistency, low-mode device rounding, zero median block difference and absent network service. Timing/file totals need controlled inputs.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

The report may mix states observed at different times and recursive disk inspection can be expensive. clean requests Java collection but guarantees neither collection nor a particular RAM reduction.

## Check your understanding

Why can status complete:true fail on a no-tip node even though normal status has explicit zero-value output?

Pinned Java SHA-256: `8d0247e53440969ace576d46015982e7dfb75c86825160e3e7877f55eab4e0eb`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
