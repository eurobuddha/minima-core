# P2PPeersChecker — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/P2PPeersChecker.java](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PPeersChecker.java) | [src/org/minima/system/network/p2p/P2PPeersChecker.cpp](../../../../../../../../src/org/minima/system/network/p2p/P2PPeersChecker.cpp) |

## What this file does

Maintain candidate and verified peers by checking their greetings against network mode and available local chain history.

## Follow the logic

1. The two getters expose live sets. Admission ignores known candidates, caps the unverified set, and when verified capacity is full admits a new candidate only on the source’s roughly ten-percent random branch. Admitting queues a check.
2. ADDPEERS retrieves the local-address cache even though its returned set is unused, applies prefix/IPv6 checks, and permits a bypass with ALLOW_ALL_IP.
3. CHECKPEERS reads optional force and forcelog separately. Force permits a ping without any connected clients; otherwise a disconnected node schedules a one-minute retry without pinging.
4. A greeting must match TEST versus non-TEST mode and contain nonempty 50block/50hash fields. With enough local history, the referenced node must exist and have the same hash. No local tip or a tip below that height logs inability to check but does not itself invalidate the greeting.
5. Success removes the unverified entry, may randomly evict a verified peer and notify its removal, then removes/readds this address and posts P2P_ADD_PEER. Failure of a previously verified peer moves it to unverified and schedules a thirty-minute retry; failure of an unverified peer removes it outright. Both failure paths notify the parent.
6. LOOP queues checks when online and always reschedules six hours later. FORCEFULLCHECK enables forcelog on queued checks, not force: it does not bypass the connected-client prerequisite. INIT has no active work.

## How the translation is approached

C++ reuses SocketAddressSet, actual NIO ping and TxPoWTreeNode lookups. A weak parent receiver prevents the parent/checker ownership cycle; database tip access is explicit. Random admission/eviction remains independent across runtimes, and missing receivers may fail after earlier set mutations.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/peers_checker_differential.py). PeersCheckerOracle and peers_checker use Java-created chain identities and a real loopback responder carrying actual serialized Greetings. They compare exact request bytes, traffic, sets, parent messages, timer delays and log bodies for good/wrong/future/missing chain data, test-mode mismatch and receiver failures. Random admissions are checked statistically and by state invariants; arbitrary multi-peer random eviction order is not claimed. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxPoWTreeNode](../../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [Greeting](../../../../../../../../../upstream-study/src/org/minima/objects/Greeting.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [NIOManager](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java), [MessageProcessor](../../../../../../../../../upstream-study/src/org/minima/utils/messages/MessageProcessor.java), [TimerMessage](../../../../../../../../../upstream-study/src/org/minima/utils/messages/TimerMessage.java).

Method navigation (declarations with bodies found in this file): [getUnverifiedPeers](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PPeersChecker.java#L53), [getVerifiedPeers](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PPeersChecker.java#L59), [P2PPeersChecker](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PPeersChecker.java#L67), [haveAnyPeers](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PPeersChecker.java#L84), [checkUnverifiedPeer](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PPeersChecker.java#L95), [processMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PPeersChecker.java#L129), [removeRandomItem](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PPeersChecker.java#L351).

## Check your understanding

Why does FORCEFULLCHECK still schedule a one-minute retry when no peers are connected? Distinguish force from forcelog.

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `76a3d02b0313bafbe685b2e0a90ba3f540f39e72ac0a7b09ee062a080b55a766`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
