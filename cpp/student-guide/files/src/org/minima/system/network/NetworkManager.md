# NetworkManager — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/NetworkManager.java](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java) | [src/org/minima/system/network/NetworkManager.cpp](../../../../../../../src/org/minima/system/network/NetworkManager.cpp) |

## What this file does

Own discovery, NIO transport and optional RPC services, select the host address and summarize their status.

## Follow the logic

1. Construction calculates the host, starts P2P or a dummy processor, creates NIO, then starts P2P2 or its dummy, and finally optional RPC. This source order matters when startup fails after earlier workers have begun.
2. Host selection returns immediately for an explicitly configured host. Otherwise start at 127.0.0.1, skip down/loopback interfaces, accept IPv4-looking addresses and prefer the first one on an interface whose display name starts wl. Without that preferred interface, later suitable addresses replace earlier choices.
3. Status retrieves UserDB even though the returned object is unused, then reports host/ports and pending/connected counts. Full status adds discovery configuration and formatted traffic.
4. Traffic rates divide signed byte totals by elapsed whole minutes, forcing a zero divisor to one. Negative durations retain Java Duration and integer-division semantics; the clock is not clamped to startup time.
5. startRPC creates only when absent; stopRPC shuts down before clearing its reference. shutdownNetwork sets the flag, stops RPC, posts NIO shutdown, then stops both discovery processors.
6. Completion uses ordered short-circuit checks on all three processors. hardShutDown catches the NIO operation’s Exception and then stops discovery; it is a different sequence from normal shutdown.

## How the translation is approached

C++ reuses the actual mapped managers and HTTPServer/CMDHandler. Factories establish shared ownership before workers start; weak owner callbacks avoid permanent cycles. Native destructor/join cleanup also covers partial construction. Required database/Main services remain explicit, and the owner shutdown flag is kept separate from NIO’s Main shutdown receiver.

## What the tests tell us

[Read the differential experiment](../../../../../../../tools/network_manager_differential.py). NetworkManagerOracle and network_manager compare real host-interface selection and status with actual NIO maps/traffic, signed rate extremes, configured-host bypass and unused database lookup failure order. Start-date text is normalized and elapsed values avoid live minute boundaries. This driver does not prove constructor/soft-hard lifecycle parity; native integration checks are a separate category. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [UserDB](../../../../../../../../upstream-study/src/org/minima/database/userprefs/UserDB.java), [NIOManager](../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java), [NIOTraffic](../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOTraffic.java), [P2PFunctions](../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java), [P2PManager](../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java), [P2P2Manager](../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Manager.java), [CMDHandler](../../../../../../../../upstream-study/src/org/minima/system/network/rpc/CMDHandler.java), [HTTPServer](../../../../../../../../upstream-study/src/org/minima/system/network/rpc/HTTPServer.java), [Server](../../../../../../../../upstream-study/src/org/minima/system/network/rpc/Server.java), [GeneralParams](../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MiniFormat](../../../../../../../../upstream-study/src/org/minima/utils/MiniFormat.java), [MinimaLogger](../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONObject](../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [Message](../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java), [MessageProcessor](../../../../../../../../upstream-study/src/org/minima/utils/messages/MessageProcessor.java).

Method navigation (declarations with bodies found in this file): [NetworkManager](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L55), [processMessage](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L71), [processMessage](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L89), [calculateHostIP](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L103), [getStatus](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L147), [getStatus](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L151), [startRPC](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L214), [getSocketHandler](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L221), [stopRPC](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L229), [shutdownNetwork](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L237), [isShutDownComplete](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L262), [hardShutDown](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L268), [getP2PManager](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L278), [getP2P2Manager](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L282), [getNIOManager](../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java#L286).

## Check your understanding

Why must getStatus still call getUserDB even though it never reads the returned UserDB? Consider a receiver that throws.

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `0eb26788b54a4c1258392d3a206ac01d7f07c83c245411dc7e4c5a58250b3956`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
