# P2PWalkLinks — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/messages/P2PWalkLinks.java](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java) | [src/org/minima/system/network/p2p/messages/P2PWalkLinks.cpp](../../../../../../../../../src/org/minima/system/network/p2p/messages/P2PWalkLinks.cpp) |

## What this file does

Represent a discovery walk, including its route, direction, joining/client flags, available client slots and authorization secret.

## Follow the logic

1. The default route is a new empty list and the secret has eight random bytes. The target-UID constructor records a local next hop; the path constructor retains the caller’s list and interprets its second Boolean as returning, not joining.
2. addHopToPath rejects a null new address. Direct mutation through getPathTaken still exposes the underlying list, so that restriction is not enforced on every possible insertion.
3. getPreviousNode searches for the first address equal to thisAddress and returns the element immediately before it. The first element, a missing address and an empty route return null.
4. JSON writes a walk_links wrapper with seven protocol fields. The target UID is local and omitted. fromJson applies present fields in order: secret, walk/join/client flags, slots, returning, then path.
5. A failed later field leaves earlier updates intact. Replacing the path changes which list the walk aliases; existing callers can still hold the former list.

## How the translation is approached

C++ shared vectors preserve supplied route aliases and address equality uses the Java-compatible adapter. The two Boolean constructor overloads remain distinct by their third argument. Null and bad-type errors are checked but exact Java exception types are not established by the normalized suite.

## What the tests tell us

[Read the differential experiment](../../../../../../../../../tools/peer_messages_differential.py). Walk experiments exercise all three constructors, aliases through external/returned lists, null paths, duplicate addresses, previous-hop lookup and incremental malformed JSON. Every operation records all flags, route, local target and JSON. The initial secret length is checked before a fixed secret makes comparison deterministic. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [JSONArray](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [getTargetUidForNextHop](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L14), [setTargetUidForNextHop](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L18), [P2PWalkLinks](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L32), [P2PWalkLinks](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L35), [P2PWalkLinks](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L41), [readFromJSON](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L47), [addHopToPath](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L53), [getPreviousNode](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L60), [toJson](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L70), [fromJson](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L84), [getSecret](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L111), [setSecret](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L115), [isWalkInLinks](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L119), [setWalkInLinks](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L123), [isJoiningWalk](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L127), [setJoiningWalk](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L131), [isClientWalk](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L135), [setClientWalk](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L139), [getAvailableNoneP2PConnectionSlots](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L143), [setAvailableNoneP2PConnectionSlots](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L147), [isReturning](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L151), [setReturning](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L155), [getPathTaken](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L159), [setPathTaken](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java#L163).

## Check your understanding

For route [A,B,A,C], what does getPreviousNode(A) return? Explain why searching from the end would change the protocol behavior.

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `5887710b7296cd28334ccea2cbbcac0f4abea51377ad3fb007c250767f74bdae`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
