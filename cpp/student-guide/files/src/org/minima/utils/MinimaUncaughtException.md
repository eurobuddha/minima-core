# MinimaUncaughtException — teaching guide

**Teaching status:** Authored explanation of the source algorithm, native adaptations, and the specific test evidence described below. This is not a full parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../upstream-study/src/org/minima/utils/MinimaUncaughtException.java) · [Mapped C++ body](../../../../../../src/org/minima/utils/MinimaUncaughtException.cpp) · [C++ header](../../../../../../src/org/minima/utils/MinimaUncaughtException.hpp)

## Purpose

This default-thread handler suppresses consecutive duplicate error text, logs new failures, immediately halts for three fatal categories, and delegates other errors. The default handler it captures is shared across instances, while the last-error text belongs to each instance.

## Follow the algorithm

1. Construction captures the currently installed default uncaught handler into a static field. Constructing a later instance changes the delegate observed by earlier instances too. Each new instance starts with last-error text***.
2. On a call, evaluate throwable.toString before accessing the thread. If that text equals the instance’s last error, return immediately; even a fatal type is suppressed by this branch.
3. For a new text, log the thread-name line, then logUncaught with notification enabled. Only after both logging operations succeed assign the initial text to mLastError. A logging failure leaves the previous last-error value intact.
4. Classify the throwable by type. OutOfMemoryError, MVStoreException, and TimeoutException each produce a shutdown message followed by Runtime.halt(0), which bypasses graceful shutdown.
5. Other types are passed to the captured default handler. A missing delegate fails; the source does not silently fall back to stderr.

## How the C++ translation works

The native handler uses NativeThread and the existing Throwable kind classification. Its default halt callback is std::_Exit(0); injected callbacks are a test/platform boundary. Shared pointers and short mutex scopes protect handler storage and last-error accesses, while calls and destructors run outside registry locks. Last-error is assigned at the same point after logging. The locks do not turn the entire check/log/update sequence into one atomic operation.

## Understanding the tests

The dedicated driver compares operation programs that construct multiple handlers, replace defaults, reinstall a handler as the default, change throwable text between toString calls, repeat errors, and inject null/log/delegate failures. Fatal classes with text*** must return before halt. Separate isolated child processes exercise all three fatal types directly and on real thread boundaries: the driver requires exit code 0, the expected shutdown line, and absence of RETURNED, DELEGATED, and shutdown-hook output. Thus it checks immediate halt behavior without halting the test coordinator.

- [Java uncaught-handler fixture](../../../../../../support/MinimaUncaughtOracle.java)
- [Native handler and isolated fatal child fixture](../../../../../../support/minima_uncaught.cpp)
- [Duplicate/error-order and fatal-process assertions](../../../../../../tools/minima_uncaught_differential.py)

The descriptions above come from reading the cited source and test evidence. No build or test run was performed while authoring this page; no fresh pass result is claimed.

## Limits and a useful next test

The tests use controlled throwable values and explicit frames; native/JVM automatic stack frames differ. There is no native H2 engine behind the MVStore classification yet. This guide explains the handler’s behavior, not authorization to run a production node or terminate one. Concurrent duplicate delivery still needs its own interleaving analysis.

## Check your understanding

Why might the same exception be logged again after the first logging attempt throws? Why does constructing a second handler affect delegation from the first?

Pinned Java SHA-256: `a5cf55124b4efc7de2b83349af0d8ef0476a096f8a7536ef8b5efa91fbae1eb5`. [Source mapping](../../../../../../source-map.json). [Project-wide limitations](../../../../../../REVIEW.md).
