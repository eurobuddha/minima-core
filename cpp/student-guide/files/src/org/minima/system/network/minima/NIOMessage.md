# NIOMessage — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/minima/NIOMessage.java](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOMessage.java) | [src/org/minima/system/network/minima/NIOMessage.cpp](../../../../../../../../src/org/minima/system/network/minima/NIOMessage.cpp) |

## What this file does

Decode one network payload and route its protocol action, including greeting negotiation, chain synchronization and transaction relay eligibility.

## Follow the logic

1. Read the retained payload before checking shutdown/restoring. After reading its type, suppress ID/pulse traffic during IBD synchronization and suppress TxPoW IDs on slave nodes. Trace/category accounting occurs after these gates. A finally block clears retained data even after an early return inside dispatch.
2. Greeting handling checks the TEST marker, updates host and advertised port, marks validity and posts P2P_CONNECTED. An incompatible incoming peer may receive our greeting before a two-second disconnect timer. Successful greeting handling records the address in the recent-IBD set before creating/sending IBD; this class itself does not implement the advertised thirty-minute set expiry.
3. Validate received IBD before posting it to the processor. A heavier chain triggers notification and disconnect, with an optional one-second rescue timer. Sync responses with no blocks do nothing. Repeated sync requests compare only the last requested block height and update that cache before creating the response; the bandwidth gate precedes it.
4. TxPoW ID/request messages perform database lookup and send a request or object. A received new TxPoW needs a local tip. Wrong chain ID, basic/signature failure or excessive state size disconnects; later difficulty, relay, time, size, mempool and MMR checks usually disable rebroadcast while still posting the object. A failed monotonic script instead returns before posting.
5. For eligible received blocks, request missing transaction IDs and inspect at most 512 stored ancestors. A separate ten-minute per-peer throttle permits inspecting up to 256 local tip ancestors for missing transactions; synthetic UIDs 0x00 and 0x01 skip that latter scan. DECIMAL32 rounding precedes the 0.01 difficulty-ratio decision.
6. Pulse handling rejects more than 1000 IDs and prepends missing IDs to a request list, reversing their encounter order. Requests are sent only after finding a local crossover. No crossover records invalid peer addresses and disconnects; its diagnostic port selection preserves the source’s -1 quirk.
7. General text is gated by ALLOW_GEN_MESSAGE. P2P text is decoded before checking P2P_ENABLED and is parsed as an object; absent peer greeting delays its event ten seconds. Ordinary PING only consumes data. SINGLE_PING replies with chain/peer details; fields named 50block and 50hash actually use getParent(100). Its type-name conversion remains UNKNOWN_11.
8. Archive request -1 returns an empty connectivity-test IBD; ordinary archive requests build data. ARCHIVE_SINGLE_REQ has an inactive body, and received ARCHIVE_DATA is logged then ignored. Slave TxBlock messages use database/tree fallback, optionally archive lookup for requests, post blocks before seeking a missing parent, and throttle mining after resetting the PRNG.
9. MegaMMR requests require the feature flag, decode sync data and call the external MegaMMR builder. Defined Maxima types, SINGLE_PONG and MegaMMR responses have no receive branch here and fall through to unknown-message logging. Ordinary caught exceptions are logged; decoding does not require consuming every trailing payload byte.

## How the translation is approached

C++ reuses actual TxPoWChecker, TxPoWGenerator, IBD, chain-search and binary codec implementations. Shared payload retention and a scope guard replace stream/finally ownership; explicit Main/database/discovery/processor/miner receivers expose missing runtime bindings. Decimal comparison preserves the tested Java rounding decision, and atomic/concurrent fields protect native state without establishing every source interleaving. Logging uses native diagnostic output and does not reproduce Java MINIMALOG notifications.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/nio_message_differential.py). NIOMessageOracle and nio_message execute actual dispatch with Java-generated binary payloads and controlled external receivers. They compare queued reply bytes, peer/processor/miner events, client mutations, traffic categories, retained-data cleanup and sync caches. The corpus covers all 256 type bytes, truncated envelopes, greeting repetition, JSON types, real script/signature/MMR decisions, 512/256 scan boundaries, getParent(100), and heavier-chain rescue. Explicit assertions check reply/event categories and the five difficulty-boundary relay decisions [true,false,false,false,true]. Generic caught errors and logs are excluded; no sockets, database writes or miner are started, and successful MegaMMR service binding plus exact live-clock boundaries remain open. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [TxPoWDB](../../../../../../../../../upstream-study/src/org/minima/database/txpowdb/TxPoWDB.java), [TxPoWTreeNode](../../../../../../../../../upstream-study/src/org/minima/database/txpowtree/TxPoWTreeNode.java), [Greeting](../../../../../../../../../upstream-study/src/org/minima/objects/Greeting.java), [IBD](../../../../../../../../../upstream-study/src/org/minima/objects/IBD.java), [Pulse](../../../../../../../../../upstream-study/src/org/minima/objects/Pulse.java), [TxBlock](../../../../../../../../../upstream-study/src/org/minima/objects/TxBlock.java), [TxPoW](../../../../../../../../../upstream-study/src/org/minima/objects/TxPoW.java), [MiniByte](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniByte.java), [MiniData](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniData.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [MiniString](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [TxPoWChecker](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWChecker.java), [TxPoWGenerator](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWGenerator.java), [TxPoWSearcher](../../../../../../../../../upstream-study/src/org/minima/system/brains/TxPoWSearcher.java), [MegaMMRIBD](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/mmrsync/MegaMMRIBD.java), [MegaMMRSyncData](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/mmrsync/MegaMMRSyncData.java), [megammrsync](../../../../../../../../../upstream-study/src/org/minima/system/commands/backup/mmrsync/megammrsync.java), [P2PFunctions](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java), [P2PManager](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java), [InetSocketAddressIO](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [GlobalParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GlobalParams.java), [MiniFormat](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFormat.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [JSONParser](../../../../../../../../../upstream-study/src/org/minima/utils/json/parser/JSONParser.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java), [TimerMessage](../../../../../../../../../upstream-study/src/org/minima/utils/messages/TimerMessage.java).

Method navigation (declarations with bodies found in this file): [convertMessageType](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOMessage.java#L99), [NIOMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOMessage.java#L177), [setFullAddress](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOMessage.java#L182), [setTrace](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOMessage.java#L186), [run](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOMessage.java#L192).

## Check your understanding

A TxPoW passes basic validation but fails the non-monotonic script check. Trace processing and broadcasting separately. Why would dropping it immediately be a different algorithm?

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `9fd5c260c825d6aef7fe63593790ac9e22792290da5a67deb54af23f896e96be`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## Block-ID iteration follow-up

The transaction-request and pulse loops now retain their block-ID list and each current ID through a checked range. Network/database callbacks can no longer invalidate a raw vector iterator used by those loops. See the [iteration lab](../../../../../../../new-tests/txbody-id-iteration.md) for the source rules, actual consumer test, current validation and remaining limits.


## Later live delivery observation

The packaged native node passed a fresh peer-tip check, missed an immediate
150-second reopen target, then caught up and followed new blocks on a later restart.
This does not alter the original method-level evidence or certify full runtime
parity. The [live reopen lab](../../../../../../../new-tests/live-reopen-validation.md) explains the observed
limits, relevant source guards, inconclusive Java control and next experiment.
