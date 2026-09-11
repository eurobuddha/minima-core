# NIOManager — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/minima/NIOManager.java](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java) | [src/org/minima/system/network/minima/NIOManager.cpp](../../../../../../../../src/org/minima/system/network/minima/NIOManager.cpp) |

## Peer-command integration update

The [peer-command lab](../../../../../../../new-tests/network-peer-commands.md) adds actual Main-owned command execution and typed missing-NIO checks. The owner lookup now throws JavaNullPointerException for an absent binding or NIO owner instead of a generic runtime failure. The 222-program comparison passes in both builds; full live startup and delivery remain separate work.

## What this file does

Coordinate listener events, connection retries, greeting exchange, message workers, traffic categories and synchronization requests.

## Follow the logic

1. Construction owns the server, pending-client map, traffic counters and a four-worker executor. SERVERSTARTED first initializes P2P, schedules valid configured endpoints after ten seconds, then schedules the two-minute stale-read checker.
2. Ignore ordinary messages while Main is shutting down or restoring; SHUTDOWN still executes. CONNECT rejects known-invalid peers, creates a pending client and posts ATTEMPT. ATTEMPT checks that its UID is still pending before starting an asynchronous socket connection.
3. RECONNECT increments attempts first. Slave mode or disabled P2P keeps retrying; ordinary mode eventually rejects a peer when alternatives exist. An otherwise isolated connection resets its attempt count. Invalid peers cancel retries, while permitted retries receive a thirty-second timer.
4. DISCONNECTED clears both synchronization timestamps. Validity, direction, invalid-peer membership and the requested reconnect flag determine retry; slave mode can force an outgoing reconnect when no other connection to that full address exists. A retry creates a new pending client, and P2P receives the old client plus the final decision.
5. NEWCONNECTION removes its pending UID and rejects an invalid peer. An outgoing unsent greeting marks sent=true before creating/serializing the greeting. Incoming peers wait. Received payloads become NIOMessage tasks executed in the worker pool.
6. TXPOWREQ sends an ID request only when the database lacks it. SYNCTXBLOCK uses the last archive block or a second root lookup, skips height one and applies the configured age window for non-archive nodes. The stale checker disconnects reads older than ten minutes and schedules five-second reconnects only for valid outgoing peers.
7. createNIOMessage writes the type byte and object bytes, without the outer length frame. sendNetworkMessage chooses one UID or broadcast and records a type/category count after sending. sendPingMessage adds the outer MiniData frame, performs a timed exchange and decodes a Greeting after discarding, rather than validating, the reply type.
8. Normal shutdown stops/awaits workers, requests server shutdown, waits for its run loop and stops processing. hardShutDown follows a shorter path. Disconnect-all queues pending and connected removals; optional P2P removal uses advertised identity separately from the socket address.

## How the translation is approached

C++ reuses NIOServer, FixedTaskExecutor, actual stream codecs and shared client objects. Factory startup establishes ownership before threads run; weak callbacks avoid owner cycles, while connector work retains its manager. Explicit service receivers represent Main/database/discovery operations that still need full-node binding. Java signed arithmetic is reproduced for archive windows; generic log/exception details and exceptional ping cleanup remain parity limits.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/nio_manager_state_differential.py). NIOManagerStateOracle allocates an unstarted original manager and injects controlled Main/database/P2P objects. The native fixture similarly avoids connector startup. Operations compare pending/connected identity, attempt counts, packets, greeting flags, sync erasure, posted events, timers, traffic and lookup order, including failures after mutations. Assertions require packet types 0/3/13, connection timers of 5/10/30 seconds, the 120-second checker and four P2P event categories. Reconnect matrices include slave/invalid/alternative peers; archive fixtures include a root disappearing on its second lookup. Real connector dispatch, worker scheduling and complete shutdown are outside this state experiment. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [ArchiveManager](../../../../../../../../../upstream-study/src/org/minima/database/archive/ArchiveManager.java), [Greeting](../../../../../../../../../upstream-study/src/org/minima/objects/Greeting.java), [TxBlock](../../../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java), [TxPoW](../../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [MiniByte](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [connect](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/connect.java), [NetworkManager](../../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java), [P2PFunctions](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java), [P2PManager](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MiniFormat](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFormat.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Streamable](../../../../../../../../../upstream-study/src/org/minima/utils/Streamable.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java), [MessageProcessor](../../../../../../../../../upstream-study/src/org/minima/utils/messages/MessageProcessor.java), [TimerMessage](../../../../../../../../../upstream-study/src/org/minima/utils/messages/TimerMessage.java).

Method navigation (declarations with bodies found in this file): [NIOManager](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L129), [getNIOServer](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L144), [getNumberOfConnectedClients](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L148), [getNumberOfConnnectingClients](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L152), [getAllConnectionInfo](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L156), [getAllValidConnectedClients](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L178), [getAllValidOutGoingConnectedClients](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L193), [getAllConnectedDetails](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L208), [checkConnected](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L237), [getMaximaUID](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L261), [getNIOClient](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L272), [getNIOClientFromUID](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L283), [hardShutDown](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L297), [getTrafficListener](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L309), [processMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L314), [connectAttempt](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L864), [run](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L869), [disconnect](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L903), [disconnect](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L907), [sendNetworkMessageAll](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L947), [sendNetworkMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L951), [createNIOMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L985), [sendPingMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java#L1049).

## Check your understanding

If greeting creation fails after setSentGreeting(true), should another NEWCONNECTION retry the greeting? Trace the retained flag before deciding.

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `34724dfafbb4159b894487b23bf071ca89c446ee69d0430c923234784402aa6b`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## Runtime sleep follow-up

The 50 ms server-completion wait now uses NativeThread and retains propagation to the caller. The Main restart comparison starts and shuts down a real replacement NIO network on port 0. It does not directly interrupt this NIO shutdown loop; executor and I/O interruption remain separate validation work. See the [runtime sleep lab](../../../../../../../new-tests/runtime-sleep-interruption.md) for the experiment, results and limits. Full parity remains not_verified.

## Executor interruption follow-up

The [executor wait lab](../../../../../../../new-tests/executor-interruption.md) checks
the actual hard and message-driven shutdown methods with real held executor tasks.
Shutdown closes the pool first; an active wait consumes a pending interrupt and
throws before the server or processor is stopped. An already terminated empty pool
returns immediately without consuming the flag, so shutdown continues. Four NIO
cases are included in 37 passing executor comparisons / 273 assertions per build.
The server is unstarted: this proves the tested effect order, not socket shutdown
or complete manager lifecycle parity.
