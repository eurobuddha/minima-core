# P2P2Functions — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p2/P2P2Functions.java](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java) | [src/org/minima/system/network/p2p2/P2P2Functions.cpp](../../../../../../../../src/org/minima/system/network/p2p2/P2P2Functions.cpp) |

## What this file does

Provide the second discovery subsystem’s connection helpers with its own invalid-peer set and local-address cache.

## Follow the logic

1. The hostname cache and invalid-peer set belong to P2P2 independently of the original P2P class. Fallback interface names and address-prefix/colon checks follow the same source rules.
2. checkConnect rejects its own invalid entries, multiple-colon endpoints and disallowed local prefixes before checking pending connections. Matching already connected clients are not rejected by the pending guard.
3. Permitted checks post NIO_CONNECT; direct connect and disconnect go straight to NIOManager. Queries return all or only connected client-info records.
4. P2P2 logs its local-rejection and successful-request messages without the original P2P debug-level gate. The availability probe still opens and closes the source HTTPS URL.

## How the translation is approached

C++ reuses identical P2PFunctions transport/query/parser implementations while retaining separate P2P2 sets and cache plus distinct policy log text. This is selective reuse of compatible behavior, not aliasing the two subsystems’ mutable state.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/p2p2_runtime_differential.py). The helper groups reuse the original connection-policy corpus but invoke actual P2P2 methods. An explicit independence operation mutates the old subsystem and verifies the new cache/set remain separate. Pending/connected queries, availability and queue/log effects are observed with controlled external receivers. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [NIOClientInfo](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java), [NIOManager](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java).

Method navigation (declarations with bodies found in this file): [getLocalAddresses](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L56), [addInvalidPeer](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L76), [isInvalidPeer](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L85), [clearInvalidPeers](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L90), [isIPv6](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L94), [isIPLocal](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L98), [isNetAvailable](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L110), [connect](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L126), [checkConnect](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L136), [disconnect](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L186), [getAllConnections](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L195), [getAllConnectedConnections](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L199), [getNIOCLientInfo](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L213), [getAllNetworkInterfaceAddresses](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L225), [main](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p2/P2P2Functions.java#L237).

## Check your understanding

If old P2P marks an endpoint invalid, is it automatically invalid for P2P2? Identify the fields that make the answer precise.

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `3e6cbcd9b08bcf79997499aa14c996089d6b4f90063d5968e88e6b740bbae39d`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
