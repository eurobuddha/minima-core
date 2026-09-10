# trace — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`. The native command compiles and has finite differential coverage.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/trace.java) · [Native C++ body](../../../../../../../../src/org/minima/system/commands/base/trace.cpp)

## Purpose

trace changes shared MessageProcessor tracing and optionally low-level NIO client/server tracing.

## Follow the command

1. Read enable directly from the parameter map and cast to String. A null or missing value raises Must specify enable. Unlike getParam, this read does not trim; only raw text true enables tracing.
2. Parse filter with default empty text and call MessageProcessor.setTrace(on, filter).
3. Then parse network with default false and assign the same value to NIOClient.mTraceON and NIOServer.mTraceON. Omitting network resets both to false.
4. Return enabled/filter/shownetwork. If network parsing fails, the MessageProcessor trace change has already occurred. The help says case sensitive, but the actual MessageProcessor filter lowercases both sides.

## Translation and dependencies

The wrapper reuses Command parameter and reply helpers, MessageProcessor's shared trace state, and the existing atomic NIOClient and NIOServer flags. Direct string access preserves the raw enable value; getBooleanParam retains the source's trimmed network semantics. The order of writes matches Java, including changes left behind when a later getter throws.

## Understanding the tests

The trace/clock corpus runs 209 programs and 652 operations through actual registered commands and the Main fixture in each build. Trace cases cross missing, null, non-string, blank, padded, Unicode and ordinary values with two prior states. Snapshots inspect the real shared state and actual matcher after both successful and failed calls.

An independent assertion starts with both NIO flags enabled, then supplies network:null. The command fails, but its earlier trace/filter update remains and both NIO flags retain their prior values. Another assertion proves that omitting network clears both flags. A mixed-case matcher test checks the implementation rather than trusting the help text. Calling network loggingoff also demonstrates that shared MessageProcessor tracing and low-level NIO flags are separate settings.

Both full builds pass. All eleven affected regression tests pass in normal and ASAN/UBSAN builds. See the [trace and clock lab](../../../../../../../new-tests/trace-and-clock-commands.md) for the corpus, evidence and comparison limits.

## Limits

Workers stay unstarted. State observations establish these ordered command effects, not concurrent worker scheduling or full-node integration. The comparator excludes enhanced Java null/cast diagnostic wording; other compared values retain their existing comparison rules.

## Check your understanding

Why do enable:" true " and network:" true " have different behavior when runCommand receives those raw parameter strings?

Pinned Java SHA-256: `bd7e00f83cb65a9a608c08c3e92e03131dc3e16b179e2d4264eea1b48493d6f7`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
