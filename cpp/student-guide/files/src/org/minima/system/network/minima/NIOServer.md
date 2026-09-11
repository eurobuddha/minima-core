# NIOServer — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/minima/NIOServer.java](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java) | [src/org/minima/system/network/minima/NIOServer.cpp](../../../../../../../../src/org/minima/system/network/minima/NIOServer.cpp) |

## What this file does

Own the nonblocking listener and connected-client registry, using selector events to move framed bytes and report connection lifecycle events.

## Follow the logic

1. Construction only stores configuration and creates queues. run marks itself running, opens/binds the listener and selector, then posts SERVERSTARTED. A bind IOException logs and halts the process with exit code zero; other outer exceptions finish the run method.
2. register and disconnect append under their respective queue locks before waking the selector. Before run creates that selector, waking fails after the queue has already changed.
3. After each select, drain registrations first and explicit disconnections second. Explicit removal disconnects the client, removes its registry entry and posts DISCONNECTED with reconnect=false.
4. Accept configures a nonblocking channel with TCP_NODELAY and KEEPALIVE, registers it, constructs/attaches a client, stores the client and posts NEWCONNECT. Read and write readiness are independent checks on the selected key.
5. An ordinary per-client I/O failure closes and removes the client and posts DISCONNECTED. Its reconnect flag is the opposite of incoming: outgoing failures request a retry, incoming failures do not.
6. Single sends look up one UID; broadcasts visit only clients with valid greetings. shutdown sets a flag and wakes the selector. Normal loop exit closes sockets but retains client-map entries; getAllNIOClients returns a new list of those same client objects.

## How the translation is approached

C++ reuses NIOClient framing and shared socket/selection-key registrations. SocketWakeup plus poll replaces the Java selector; an IPv6 dual-stack listener has an IPv4 fallback. Atomics and queue/selector locks protect native threads. Weak manager callbacks, explicit closing and getBoundPort support lifetime management and observation; custom JVM networking settings remain outside this API translation; the source demo main is covered by the follow-up below.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/nio_server_differential.py). NIOServerOracle and nio_server use real loopback IPv4/IPv6 sockets and a controlled manager collector. They compare zero-, one-, 8193- and 65536-byte frames, fragmented headers, direct sends, valid-only broadcasts, explicit removal, outgoing EOF, incoming oversized-frame rejection and retained identity after shutdown. Expected traffic totals are asserted at 73750 read and 73760 written bytes for that scenario. Invalid ports and an isolated bind-collision child exercise return versus halt behavior; these finite traces do not prove all concurrent schedules. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java), [MessageProcessor](../../../../../../../../../upstream-study/src/org/minima/utils/messages/MessageProcessor.java).

Method navigation (declarations with bodies found in this file): [NIOServer](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L41), [shutdown](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L48), [getNetClientSize](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L55), [getAllNIOClients](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L59), [regsiterNewSocket](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L73), [disconnect](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L81), [getClient](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L89), [sendMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L93), [sendMessageAll](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L104), [isRunning](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L120), [run](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L125), [addChannel](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L307), [main](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L347), [processMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L351), [processMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOServer.java#L369).

## Check your understanding

Why does pre-start disconnect leave a queued UID despite throwing? Contrast that with shutdown, which checks whether a selector exists before waking it.

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `0a3d023c8e9baa8a4d9d2139a5fefbea7336c2027b6423f7043c1273aafea9ca`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## Standalone main follow-up

The [server main lab](../../../../../../../new-tests/nio-server-main.md) explains the
new native entry point and its actual Java/C++ process experiment. The original
program starts/stops port 9002 twice, retains four two-second waits and exits through
shutdown hooks. Real hello workers log SERVERSTARTED, then fail their finally cleanup
when no MinimaDB exists; the native implementation preserves that source behavior.
Each wait is interrupted in a separate case. The flag is consumed and a server
already running remains alive after the caller throws; a previously stopped server
closes. An occupied-port child halts without executing a shutdown hook.

Six scenarios / 103 assertions pass in each normal and sanitizer build. The actual
loopback lifecycle and 63 error-boundary cases also pass. Only fixture-owned child
processes are stopped. Complete server/thread/API parity remains unverified.
