# trace — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/trace.java) · `src/org/minima/system/commands/base/trace.cpp` (absent)

## Purpose

trace changes shared MessageProcessor tracing and optionally low-level NIO client/server tracing.

## Follow the command

1. Read enable directly from the parameter map and cast to String. A null or missing value raises Must specify enable. Unlike getParam, this read does not trim; only raw text true enables tracing.
2. Parse filter with default empty text and call MessageProcessor.setTrace(on, filter).
3. Then parse network with default false and assign the same value to NIOClient.mTraceON and NIOServer.mTraceON. Omitting network resets both to false.
4. Return enabled/filter/shownetwork. If network parsing fails, the MessageProcessor trace change has already occurred. The help says case sensitive, but the actual MessageProcessor filter lowercases both sides.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse MessageProcessor and NIO global trace state. Preserve the different raw enable access and trimmed network helper semantics.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed tests compare missing enable, raw true versus padded true, absent/present filter, and omitted network after it was previously enabled. Inject a network parsing failure and assert the earlier MessageProcessor setting remains changed.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

The native MessageProcessor worker fixture covers one lowercase filter case; it does not implement this absent command wrapper or prove NIO flag propagation.

## Check your understanding

Why do enable:" true " and network:" true " have different behavior when runCommand receives those raw parameter strings?

Pinned Java SHA-256: `bd7e00f83cb65a9a608c08c3e92e03131dc3e16b179e2d4264eea1b48493d6f7`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
