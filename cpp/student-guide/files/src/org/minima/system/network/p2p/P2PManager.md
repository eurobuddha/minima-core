# P2PManager — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/P2PManager.java](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java) | [src/org/minima/system/network/p2p/P2PManager.cpp](../../../../../../../../src/org/minima/system/network/p2p/P2PManager.cpp) |

## What this file does

Coordinate the original discovery subsystem: initialization, connection categories, JSON message routing, periodic balancing and peer persistence.

## Follow the logic

1. Construction creates state, optionally applies test parameters, creates the checker and schedules loop/connectivity/save timers. INIT later loads saved peers, records their initial count, optionally parses configured addnodes only when empty, and queues forced checker requests before selecting a connection.
2. Initialization copies host/no-connect/capacity settings into state. Connection preference is the source firstgo branch, configured root, random saved peer, then discovery. Discovery requires its state flag and a running processor; it tries at most three pings, waits five seconds after null replies, decodes peers-list, and populates known plus verified sets on success. With no defaults it rate-limits the explanatory log.
3. CONNECTED inserts allLinks before invoking swap helpers; DISCONNECTED removes allLinks then categories. NOCONNECT removes the failed address and moves matching inbound entries to not-accepting using allLinks values.
4. JSON routing first resolves the client. Missing clients request disconnect. The swap_links_p2p object uses independent ordered if blocks for greeting, IP request/response, not-accepting, walk and swap, so one object can trigger several actions. DoSwap schedules connect then disconnect; no extra secret-verification gate exists in this branch.
5. The periodic loop chooses a delay based on outgoing capacity, removes self from known peers, and connects directly, grows outgoing links, requests inbound links or balances clients as appropriate. Empty known peers trigger saved-peer checks and discovery. Connectivity assessment marks a node non-accepting only for the source’s particular empty/nonempty category combination.
6. Returned walks are routed toward their start or consumed there; outgoing walks dispatch by direction. sendMessages executes returned actions in list order, wrapping JSON under swap_links_p2p and ignoring null messages. Earlier effects remain if a later send fails.
7. Health checks clear nonessential queued messages only above fifty, preserving the three named timer types. Persistence replaces stored peers only when nonempty and at least half the initial count. Shutdown stores version, updates peers, stops checker, logs, then stops itself. Status and haveAnyPeers use their explicit query/short-circuit order.

## How the translation is approached

The translation reuses P2PState, checker, walk/swap helpers, P2PDB, NIOManager and JavaRandom48. Factories create shared ownership before workers start and the checker holds a weak parent. Database save/tip/time callbacks are explicit runtime boundaries. State constructed before test-parameter application keeps that source ordering; independent entropy and shuffled peer lists are compared only within stated scope.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/p2p_manager_differential.py). P2PManagerOracle and p2p_manager manually execute actual manager methods on unstarted fixtures, capturing state, checker queues, NIO packets, timers, logs and DB changes after each operation. Cases cover parsing, capacity/persistence thresholds, overload at fifty, malformed multi-field JSON, signed delays, null maps and partial failures. Packet lengths remain checked while random secrets and shuffled known peers are canonicalized. The separate p2p_discovery_differential.py uses actual loopback ping bytes and real retry pauses, including dropped replies and the three-attempt cap. Neither fixture alone proves complete Main lifecycle integration. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [Greeting](../../../../../../../../../upstream-study/src/org/minima/objects/Greeting.java), [connect](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/connect.java), [NIOClient](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClient.java), [NIOClientInfo](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java), [NIOManager](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java), [InetSocketAddressIO](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java), [P2PDoSwap](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java), [P2PGreeting](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java), [P2PWalkLinks](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java), [P2PParams](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/params/P2PParams.java), [P2PTestParams](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/params/P2PTestParams.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java), [MessageProcessor](../../../../../../../../../upstream-study/src/org/minima/utils/messages/MessageProcessor.java), [TimerMessage](../../../../../../../../../upstream-study/src/org/minima/utils/messages/TimerMessage.java).

Method navigation (declarations with bodies found in this file): [P2PManager](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L72), [getPeersChecker](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L97), [getPeersCopy](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L101), [getP2PAddress](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L105), [getClients](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L109), [init](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L115), [doDiscoveryPing](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L225), [processMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L276), [assessConnectivity](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L389), [processJsonMessages](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L401), [processLoop](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L458), [processWalkLinksMsg](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L505), [processReturningMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L516), [processOutgoingWalkMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L533), [connect](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L543), [getStatus](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L550), [sendMessages](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L585), [updateP2PPeersList](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L605), [shutdown](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L618), [haveAnyPeers](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java#L638).

## Check your understanding

A greeting and notAcceptingMsg arrive in the same wrapper. Trace both blocks and explain why replacing the independent if statements with else-if changes final state.

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `8e101225ae794c10e859e24dd3dc3f81e7ac3b716ac0739ba9376e896a3c7a51`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## Runtime sleep follow-up

Discovery pauses now use NativeThread. Four real TCP comparisons cover INIT/LOOP with immediate success or a dropped reply followed by success. Immediate success retains the interrupt bit; the interrupted retry pause consumes it, advances attempts and permits the second ping. The existing broader native catch for Java Error categories remains a separate gap. See the [runtime sleep lab](../../../../../../../new-tests/runtime-sleep-interruption.md) for the experiment, results and limits. Full parity remains not_verified.
