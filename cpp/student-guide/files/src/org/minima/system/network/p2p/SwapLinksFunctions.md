# SwapLinksFunctions — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/SwapLinksFunctions.java](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/SwapLinksFunctions.java) | [src/org/minima/system/network/p2p/SwapLinksFunctions.cpp](../../../../../../../../src/org/minima/system/network/p2p/SwapLinksFunctions.cpp) |

## What this file does

Turn connection and greeting events into discovery state transitions, authenticated IP discovery and capacity-balancing walks.

## Follow the logic

1. onConnected inserts the peer into non-P2P links first. Incoming connections use host with port zero for duplicate detection; duplicates produce a disconnect message but are still inserted. Other connections produce a greeting and, if the host is unknown, a new twelve-byte IP-request secret.
2. processGreeting removes the provisional entry, marks the actual NIO client’s greeting flag when Main exists, then categorizes an accepting peer into in/out links or a non-accepting peer into the not-accepting map. Excess outgoing capacity posts disconnect after insertion.
3. An accepting greeting clears the returned noconnect flag. A discovery connection clears its state flag and disconnects after classification. The computed addtoknown Boolean is unused; this method does not add that address to known peers.
4. Disconnect removes the UID from four maps in source order. Failure in a later map does not undo earlier removals; allLinks is not touched here.
5. IP requests echo the secret and sender host. Responses change the local address, mark host set and remove self from known peers only when the secret matches.
6. Scale-out and scale-in requests select a current outgoing neighbor and create walks with different direction/join flags. Load balancing requires excess non-accepting peers plus inbound links; its number of identical walk messages uses integer division by max P2P capacity.

## How the translation is approached

Shared state, existing message codecs and actual NIO client flags are reused. A required Main-presence receiver distinguishes absent Main from a missing manager. Native wrapP2PMsg retains its input JSON alias; OS-generated secrets and shuffle order are not forced to match Java randomness.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/swap_links_differential.py). The shared fixture captures complete peer maps, NIO greeting flags, disconnect queues and returned message JSON after each call. It tests duplicate incoming peers, greeting capacity boundaries, partial map failures, IP-secret mismatch, invalid ports, zero divisors and wrapper aliasing. Random secret lengths/equality relationships and known-peer permutation are retained while bytes/order are canonicalized. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [NIOClient](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClient.java), [NIOClientInfo](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java), [P2PGreeting](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PGreeting.java), [P2PWalkLinks](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PWalkLinks.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java).

Method navigation (declarations with bodies found in this file): [wrapP2PMsg](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/SwapLinksFunctions.java#L18), [onConnected](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/SwapLinksFunctions.java#L34), [onConnectedLoadBalanceRequest](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/SwapLinksFunctions.java#L74), [onDisconnected](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/SwapLinksFunctions.java#L92), [processGreeting](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/SwapLinksFunctions.java#L103), [processRequestIPMsg](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/SwapLinksFunctions.java#L154), [processResponseIPMsg](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/SwapLinksFunctions.java#L164), [joinScaleOutLinks](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/SwapLinksFunctions.java#L177), [requestInLinks](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/SwapLinksFunctions.java#L191).

## Check your understanding

Why must a duplicate incoming connection still appear in noneP2PLinks even though its returned action requests disconnection?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `e6b507d9ca240df277b4a5caaa115e333d85b64873638231c0a6f0982e748f34`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
