# WalkLinksFuncs — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/WalkLinksFuncs.java](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/WalkLinksFuncs.java) | [src/org/minima/system/network/p2p/WalkLinksFuncs.cpp](../../../../../../../../src/org/minima/system/network/p2p/WalkLinksFuncs.cpp) |

## What this file does

Route discovery walks, turn completed walks into connection attempts and generate swap requests when link capacity is exceeded.

## Follow the logic

1. Filter a chosen link map against every address already in the route. Incoming/outgoing walk handling first appends the sender address, then selects a remaining neighbor. Continue only while route length is below nine and a next hop exists.
2. An exhausted incoming-link walk appends this node, marks returning and, for a client walk, records max non-P2P capacity minus current not-accepting peers. Response routing uses the previous occurrence in the recorded route.
3. An exhausted outgoing-link walk can request an inbound swap only when inLinks exceeds the target. The source chooses nextInt(numberOfKeys-1), excluding the final key and failing when that bound is zero. The original secret and first route address are reused.
4. A returned scale-out walk attempts the last route address while below target, stops after a permitted connection, and removes the attempted address by value. With duplicates, remove deletes its first equal occurrence rather than necessarily the final element.
5. A returned load-balance walk uses its last address unless it is this node. When overloaded, generate at most min(available remote slots, local maximum/2) messages for the first selected map keys, with new eight-byte secrets.
6. Helpers may return null, an empty list, or messages. Connection requests are also posted as side effects; an empty returned list does not mean no work occurred.

## How the translation is approached

C++ reuses shared route lists, peer map enumeration, JavaRandom48 and DoSwap/WalkLinks codecs. Separate seeded random streams preserve selection order; signed slot subtraction is explicit wraparound. Actual NIO connection posting remains visible through the existing manager.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/walk_links_differential.py). P2PFunctionsOracle and p2p_functions execute every walk helper and snapshot route mutations, return flags, slots, logs and posted connections. Cases vary lengths around nine, zero/one/many keys, duplicate paths, null receivers and signed capacities. Explicit assertions require one-key swap selection to fail and successful choices to exclude the final key. Only newly random secret bytes are canonicalized while lengths/reuse are retained. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [NIOClientInfo](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java), [P2PDoSwap](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java), [P2PWalkLinks](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java).

Method navigation (declarations with bodies found in this file): [WalkLinksFuncs](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/WalkLinksFuncs.java#L25), [removeIPsInBFromA](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/WalkLinksFuncs.java#L36), [onOutLinkWalkMsg](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/WalkLinksFuncs.java#L59), [createNextHopMsg](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/WalkLinksFuncs.java#L84), [generateDoSwapMessageFromWalk](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/WalkLinksFuncs.java#L106), [getAddressFromUID](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/WalkLinksFuncs.java#L124), [onInLinkWalkMsg](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/WalkLinksFuncs.java#L158), [onWalkLinkResponseMsg](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/WalkLinksFuncs.java#L188), [onReturnedWalkMsg](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/WalkLinksFuncs.java#L213), [onReturnedLoadBalanceWalkMsg](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/WalkLinksFuncs.java#L236), [genLoadBalanceDoSwaps](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/WalkLinksFuncs.java#L250).

## Check your understanding

For [A,B,A], trace the first failed attempt at A and the list after remove(A). Why is pop_back an incorrect translation?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `feed570a737af06e44ec83aae3e710a3aa347110d979acd409ae15b6dc5b9ec4`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
