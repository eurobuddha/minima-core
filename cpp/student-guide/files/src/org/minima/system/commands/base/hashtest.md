# hashtest — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/hashtest.java) · `src/org/minima/system/commands/base/hashtest.cpp` (absent)

## Purpose

hashtest times the miner’s hash-speed calculation and reports an approximate rate in megahashes per second.

## Follow the command

1. Parse amount as MiniNumber with a default of one million.
2. Read wall-clock milliseconds, call TxPoWMiner.calculateHashSpeed(hashes), then read the clock again and subtract for elapsed milliseconds.
3. Log the returned raw speed. Divide speed by one million and set four significant digits, then append the literal MH/s unit.
4. Return original hashes as a MiniNumber JSON value, elapsed milliseconds, and the formatted speed. The command itself does not perform the hash loop.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse the miner’s actual speed routine, MiniNumber arithmetic and logger. A timed no-op would exercise the response format but not hash performance.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed tests inject controlled clock/speed results to check units, significant digits, logging order and failure behavior. Separately run actual hashing to verify work is performed; do not require Java and C++ wall times to be equal.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

Benchmark speed varies with runtime and machine load. Timing equality is not behavioral parity, and fixed-format tests do not establish performance.

## Check your understanding

Why is a deterministic formatting test more appropriate than asserting equal elapsed time in Java and C++?

Pinned Java SHA-256: `bd24950bae8b1a51f435518157a0be9fc76b52cd974414134748697c76fdf639`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
