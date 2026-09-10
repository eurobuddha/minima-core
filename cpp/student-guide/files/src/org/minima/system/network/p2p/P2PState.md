# P2PState — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/P2PState.java](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java) | [src/org/minima/system/network/p2p/P2PState.cpp](../../../../../../../../src/org/minima/system/network/p2p/P2PState.cpp) |

## What this file does

Hold discovery’s mutable connection categories, known-peer set, local address and control flags. It is state storage; it does not reconcile the categories automatically.

## Follow the logic

1. Construction creates five separate maps, an empty peer set, wildcard port 9001 address, default MiniData secret and scalar values copied from current P2PParams.
2. Map setters replace references and getters return those same maps. allLinks is independent of the four category maps; callers must maintain consistency.
3. getKnownPeers returns the live set, whereas getKnownPeersCopy creates a new list containing the current address references.
4. The string address setter uses the current GeneralParams.MINIMA_PORT. setLoopDelayToParamValue copies the current global delay; subsequent global changes do not update the state automatically.
5. toJson removes slash characters from address text, renders four lists in map iteration order, and reports allLinks and known-peer counts separately. Null/replaced maps can make output fail.

## How the translation is approached

C++ reuses JavaHashMap and SocketAddressSet to preserve address equality and tested iteration/retention behavior. Shared pointers preserve map aliases; separate mutexes protect scalar/reference replacement and peer access. These locks do not prove equivalent whole-program concurrent interleavings.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/peer_state_differential.py). State programs compare defaults, getters/setters, aliases, null replacements, map insertion/removal order, set equality/resizing, copied lists and JSON after every operation. Special non-comparable hash collision cases compare membership only. Greeting shuffle order is normalized as a permutation, and concurrent mutation is excluded. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [InetSocketAddressIO](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java), [P2PParams](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/params/P2PParams.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [P2PState](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L81), [getKnownPeers](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L85), [getKnownPeersCopy](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L89), [isAcceptingInLinks](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L97), [setAcceptingInLinks](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L101), [setInLinks](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L109), [setOutLinks](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L117), [setNoneP2PLinks](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L125), [getMaxNumNoneP2PConnections](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L129), [setMaxNumNoneP2PConnections](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L133), [isDoingDiscoveryConnection](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L137), [setDoingDiscoveryConnection](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L141), [getMyMinimaAddress](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L145), [setMyMinimaAddress](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L149), [setMyMinimaAddress](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L153), [getIpReqSecret](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L157), [setIpReqSecret](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L161), [getLoopDelay](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L165), [setLoopDelay](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L169), [setLoopDelayToParamValue](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L173), [setNotAcceptingConnP2PLinks](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L181), [toJson](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L185), [isNoConnect](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L205), [setNoConnect](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L209), [isHostSet](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L213), [setHostSet](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L217), [getMaxNumP2PConnections](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L221), [setMaxNumP2PConnections](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L225), [setAllLinks](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L233), [isStartupComplete](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L237), [setStartupComplete](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PState.java#L241).

## Check your understanding

Put a peer in inLinks only. What should numAllLinks report? Identify which component must later reconcile the two maps.

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `a852588bfd9c5a20f545acdfd3841147afc25a923645571fbde98912795a3e6c`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
