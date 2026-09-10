# HTTPServer — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/rpc/HTTPServer.java](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/HTTPServer.java) | [src/org/minima/system/network/rpc/HTTPServer.cpp](../../../../../../../../src/org/minima/system/network/rpc/HTTPServer.cpp) |

## What this file does

Accept TCP connections and delegate each accepted socket to an independently running request handler.

## Follow the logic

1. Java construction optionally starts a listener thread. run binds a ServerSocket at the configured port and logs startup before entering the running loop.
2. Each loop accepts one socket, calls getSocketHandler on the listener thread, then starts its returned Runnable in a daemon thread named Socket Handler @ followed by the configured port.
3. The handler is independent of the next accept operation. A slow request should not prevent another connection’s handler from running.
4. shutdown sets running false before closing the server socket. The resulting socket failure is logged only if the server still considers itself running.
5. BindException, SocketException and IOException have separate catch behavior. Other runtime failures and Java Errors are not caught by this source body; handler failures occur on the handler thread.

## How the translation is approached

C++ starts only after derived shared ownership exists, avoiding a worker using a half-owned object. It reuses POSIX sockets with default dual-stack binding and a wakeup pipe/poll loop so shutdown interrupts waiting reliably. Handler closures retain accepted sockets; detached threads model Java daemon handlers. A separate bound-port field supports ephemeral-port tests.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/http_server_differential.py). HTTPServerOracle and http_server use actual listener sockets and a binary reversing handler. One connection is held open while four others complete, then the held request completes; factory/completion counts and concurrent activity are observed. Automatic/manual startup, port collision logs and shutdown completion are compared. The configured port remains distinct from the normalized ephemeral port. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java).

Method navigation (declarations with bodies found in this file): [HTTPServer](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/HTTPServer.java#L17), [HTTPServer](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/HTTPServer.java#L21), [shutdown](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/HTTPServer.java#L30), [start](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/HTTPServer.java#L42), [run](../../../../../../../../../upstream-study/src/org/minima/system/network/rpc/HTTPServer.java#L50).

## Check your understanding

Identify whether an exception from getSocketHandler occurs in the listener thread or the request thread. Why does that distinction affect which loop survives?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `8b97e8f2faf4c7eb98bb7e4b98a662929c6abd9e48cedd03ca9de713ffc821d3`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
