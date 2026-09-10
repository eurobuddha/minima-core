# MinimaLogger — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/MinimaLogger.cpp) · [C++ header](../../../../../../src/org/minima/utils/MinimaLogger.hpp)

## Purpose

Logging here has two observable effects: printing a formatted line and optionally posting a MINIMALOG notification. Printing, querying Main, and notifying can all fail or change application state. Their order is therefore part of behavior, not just presentation.

## Follow the algorithm

1. Read total heap memory, then free memory, subtract using Java long arithmetic, obtain the timestamp, and format the full line as Minima @ date [size] : message. A null string is rendered as the text null.
2. Print the line first. Only afterward, if notifications are enabled, query Main.getInstance. If it is non-null, create a JSON message object, query Main again, and post MINIMALOG with that object.
3. For an Exception, call its toString and log that first. Then obtain its stack trace and log each frame with five leading spaces, passing the same notification flag each time.
4. For uncaught Throwable logging, prepend the UNCAUGHT EXCEPTION label before logging the throwable text, then process frames in the same order. The method does not swallow failures from formatting, printing, trace retrieval, or notification.

## How the C++ translation works

Native Runtime callbacks supply allocator statistics, clock, printing, and the still-unported Main receiver. The implementation retains a safe shared runtime snapshot, releases retired callbacks outside the registry lock so destructors may reenter logging, and preserves the two Main lookups. Used memory subtraction explicitly wraps as signed 64-bit Java arithmetic. Throwable text/recorded frames come from a native boundary interface rather than a fabricated catch-site stack. The date wrapper captures the default zone at initialization.

## Understanding the tests

The differential fixture controls DateFormat, PrintStream, Main, and Throwable methods and records the exact call sequence, printed strings, notifications, and counts. Inputs vary null/Unicode text, missing frames, each failure point, Main appearing/disappearing during print, nested logging, and mutation of the notification object. Four default time zones and extreme signed-long timestamps are compared. Only heap-dependent prefix values and automatic NPE wording are normalized. A native worker test separately requires 400 distinct print-before-notify pairs,800 Main lookups, destructor reentry without deadlock, and the correct failure when Main disappears on the second lookup.

- [Java logger fixture](../../../../../../support/MinimaLoggerOracle.java)
- [Native logger fixture](../../../../../../support/minima_logger.cpp)
- [Order, values, failures, and date driver](../../../../../../tools/minima_logger_differential.py)
- [Native concurrency and reentry assertions](../../../../../../support/minima_logger_worker.cpp)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

Allocator statistics naturally differ from JVM heap statistics. The fixed date-pattern surface does not implement every mutable SimpleDateFormat feature. The worker checks provide finite concurrency evidence, not a proof against every race. Full Main/node startup is still absent; receiver callbacks are controlled in these tests.

## Check your understanding

If printing removes Main, should a notification still occur? If Main exists for the first lookup but disappears for the second, why would caching the first pointer change the source behavior?

Pinned Java SHA-256: `1788078528ea03e9420b82150eb2629b2ffb5f4329b6a035debc856232ced3c7`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).
