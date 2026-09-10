# P2PDoSwap — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/messages/P2PDoSwap.java](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java) | [src/org/minima/system/network/p2p/messages/P2PDoSwap.cpp](../../../../../../../../../src/org/minima/system/network/p2p/messages/P2PDoSwap.cpp) |

## What this file does

Carry a swap authorization secret and destination through discovery JSON, while retaining a local client UID that is not transmitted.

## Follow the logic

1. A default instance creates eight random secret bytes and leaves destination and client UID null. The full constructor checks the destination before replacing fields with the supplied references.
2. toJson creates a do_swap wrapper containing secret, swap_target_host and swap_target_port. It uses getHostString, so a stored hostname can remain a hostname.
3. readJson updates the secret first if present. It replaces the destination only if both host and port keys exist; absent fields retain their old values.
4. Reading never updates the local swappingClientUID. If destination construction fails after a valid secret, the changed secret remains: parsing is incremental, not transactional.

## How the translation is approached

C++ keeps shared MiniData/address objects and an optional UID. It reuses the same JSON and address helpers, and makes the absent destination an explicit checked failure. Random bytes are generated independently in each runtime.

## What the tests tell us

[Read the differential experiment](../../../../../../../../../tools/peer_messages_differential.py). The swap programs fix the random secret after checking its length, then apply repeated partial JSON reads and setters. Each step captures result, secret, destination, local UID and generated JSON, including state after a failing read. This tests mutation order; it does not compare cryptographic randomness bit for bit. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MiniData](../../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [JSONObject](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [P2PDoSwap](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java#L20), [P2PDoSwap](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java#L24), [readFromJson](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java#L33), [getSwappingClientUID](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java#L40), [setSwappingClientUID](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java#L44), [toJson](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java#L48), [readJson](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java#L58), [getSecret](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java#L68), [setSecret](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java#L72), [getSwapTarget](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java#L76), [setSwapTarget](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/P2PDoSwap.java#L80).

## Check your understanding

Start with destination A and secret 0x01. Read a new valid secret plus an out-of-range destination port. Which old fields survive?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `7b937960c9b1fa3e1709805e93a4411fb320f24f6e3d44154e364d7682ce9d5f`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).
