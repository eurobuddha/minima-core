# P2P2Manager — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p2/P2P2Manager.java](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Manager.java) | [src/org/minima/system/network/p2p2/P2P2Manager.cpp](../../../../../../../../src/org/minima/system/network/p2p2/P2P2Manager.cpp) |

## What this file does

Translate the active behavior of an unfinished upstream discovery manager without inventing the algorithms its comments propose.

## Follow the logic

1. Construction queues INIT and schedules a five-minute fast loop and six-hour slow loop. shutdown stops the processor and logs; the declared SHUTDOWN message has no matching active branch.
2. INIT logs and adds the literal source strings 324.45.45.45:9901 twice and 324.45.45.45:9902 once through P2P2DB. These are stored peer strings, not successful network connections. It then reads first-startup even though that conditional body is inactive.
3. FAST_LOOP schedules its next timer before checking network availability. Offline returns immediately. When below the desired connection count, it selects a peer but never connects: the connection statement is commented out.
4. Random selection returns an empty string for no peers and otherwise uses a fresh Random for an index. Null entries may be selected.
5. SLOW_LOOP and checkWhichConnect contain no active work. convertOldP2P reads each old address’s host/port but neither fills nor saves a new list.

## How the translation is approached

C++ uses existing MessageProcessor, P2P2DB and JavaRandom48 with explicit database receivers. It preserves source no-ops and lookup order, including effects before later failures. Completing the intended new discovery design would be a separate behavior change, not a faithful translation.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/p2p2_runtime_differential.py). P2P2RuntimeOracle and p2p2_runtime record database access order, inserted peers, queued/timer messages, shutdown state and old database contents. They inject lookup failures and malformed first-startup data, vary online/connectivity counts and exercise inactive branches. Fresh multi-peer choices are compared by membership and distribution bounds, not identical random sequences. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java), [MessageProcessor](../../../../../../../../../upstream-study/src/org/minima/utils/messages/MessageProcessor.java), [TimerMessage](../../../../../../../../../upstream-study/src/org/minima/utils/messages/TimerMessage.java).

Method navigation (declarations with bodies found in this file): [P2P2Manager](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Manager.java#L30), [shutdown](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Manager.java#L41), [getRandomPeerFromList](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Manager.java#L51), [checkWhichConnect](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Manager.java#L67), [convertOldP2P](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Manager.java#L73), [processMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Manager.java#L89).

## Check your understanding

Why should a FAST_LOOP test expect another timer but no NIO_CONNECT message even when the node has zero connections?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `966cb0513bd17c7b7d4f38e1a48584d047c161cd76156605e8482c2dc82e1f85`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
