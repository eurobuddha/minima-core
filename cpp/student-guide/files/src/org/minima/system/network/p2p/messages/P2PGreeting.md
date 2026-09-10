# P2PGreeting — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/messages/P2PGreeting.java](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java) | [src/org/minima/system/network/p2p/messages/P2PGreeting.cpp](../../../../../../../../../src/org/minima/system/network/p2p/messages/P2PGreeting.cpp) |

## What this file does

Advertise a node’s discovery capabilities and peers. Its equals method is deliberately much weaker than full message equality.

## Follow the logic

1. The state constructor copies in/out map values and known peers, captures current port and connection limits, optionally appends the local address when accepting inbound links, then shuffles known peers.
2. The short and full explicit constructors retain supplied lists. The short constructor marks the node as not accepting inbound links.
3. fromJSON reads the inner greeting object, not its outer wrapper. It always reads port, acceptance and known peers, and only reads in/out links and client counts when accepting.
4. toJson always emits port, acceptance and known peers; it emits the extra link/count fields only for an accepting node, then wraps everything under greeting.
5. equals returns true for self before touching fields, false for other types/null, then compares only port, acceptance and known-peer count. Different peer contents can compare equal.

## How the translation is approached

C++ retains shared explicit-constructor lists and creates new state-constructor lists. Native shuffle uses independent entropy, so a seeded Java shuffle sequence is not claimed. The typed equals pointer API preserves self/null short circuits and the narrow three-field comparison.

## What the tests tell us

[Read the differential experiment](../../../../../../../../../tools/peer_state_differential.py). The fixture exercises constructors, list aliases, JSON defaults and wrong types, serialization and equality. An explicit case uses different peer contents with equal sizes, and another sets knownPeers null before self-comparison. State-constructor known peers are sorted for comparison; equality alone is never treated as proof that serialized messages match. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [P2PState](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java), [GeneralParams](../../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [JSONArray](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [P2PGreeting](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L53), [P2PGreeting](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L56), [P2PGreeting](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L74), [P2PGreeting](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L83), [fromJSON](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L95), [toJson](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L110), [getMyMinimaPort](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L126), [setMyMinimaPort](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L130), [isAcceptingInLinks](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L134), [setAcceptingInLinks](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L138), [getOutLinks](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L142), [setOutLinks](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L146), [getInLinks](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L150), [setInLinks](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L154), [getNumNoneP2PConnections](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L158), [setNumNoneP2PConnections](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L162), [getMaxNumNoneP2PConnections](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L166), [setMaxNumNoneP2PConnections](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L170), [getKnownPeers](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L174), [setKnownPeers](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L178), [equals](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java#L183).

## Check your understanding

Construct two greetings with the same port and one different known peer each. Can equals return true while their JSON differs? Show why.

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `f60e7b835f157f44995dafcddb4d4ed10b07ba37708d667a92a42e0e8e0cd96d`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
