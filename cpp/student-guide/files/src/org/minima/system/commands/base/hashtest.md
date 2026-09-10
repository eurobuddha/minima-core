# hashtest — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/hashtest.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/base/hashtest.cpp)

## Purpose

hashtest times the miner’s hash-speed calculation and reports an approximate rate in megahashes per second.

## Follow the command

1. Parse amount as MiniNumber with a default of one million.
2. Read wall-clock milliseconds, call TxPoWMiner.calculateHashSpeed(hashes), then read the clock again and subtract for elapsed milliseconds.
3. Log the returned raw speed. Divide speed by one million and set four significant digits, then append the literal MH/s unit.
4. Return original hashes as a MiniNumber JSON value, elapsed milliseconds, and the formatted speed. The command itself does not perform the hash loop.

## Translation and dependencies

The mapped native body reuses Command parameter/reply helpers, the existing miner speed routine, MiniNumber arithmetic and MinimaLogger unchanged. The elapsed subtraction uses unsigned arithmetic followed by a signed bit conversion to preserve Java long wrap without C++ signed overflow. The response stores hashes as raw numeric JSON using MiniNumber formatting, matching Java’s object serialization fallback. The first comparison caught a string-versus-number mismatch before this correction.

## Understanding the tests

The [new lab](../../../../../../../../student-guide/new-tests/hash-speed-command.md) was written before the comparison. It runs actual hashing and validates each runtime independently: bounded elapsed milliseconds, rate consistent with a bounded integer internal duration, and exact four-digit truncation of the logged speed divided by one million. Only validated timing/speed fields are normalized. Existing miner worker tests independently exercise clamped elapsed time through the miner’s clock parameter.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

Both targeted builds and the normal/sanitizer comparisons pass: 59 programs and 61 operations per build. The full regression suites already in progress use their existing binaries, so they do not certify this new command.

## Limits

Benchmark speed varies with runtime and machine load. Timing equality is not behavioral parity, and fixed-format tests do not establish performance.

## Check your understanding

Why is a deterministic formatting test more appropriate than asserting equal elapsed time in Java and C++?

Pinned Java SHA-256: `bd24950bae8b1a51f435518157a0be9fc76b52cd974414134748697c76fdf639`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
