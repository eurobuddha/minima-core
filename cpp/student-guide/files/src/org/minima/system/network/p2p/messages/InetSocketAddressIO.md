# InetSocketAddressIO — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java) | [src/org/minima/system/network/p2p/messages/InetSocketAddressIO.cpp](../../../../../../../../../src/org/minima/system/network/p2p/messages/InetSocketAddressIO.cpp) |

## What this file does

Convert peer addresses between Java socket objects and the two JSON forms used by discovery: objects with host/port fields and strings containing host:port.

## Follow the logic

1. Object output visits peers in list order and uses the resolved numeric host address plus the port. An unresolved address can therefore be constructed successfully but fail during output.
2. String output also uses the resolved numeric address, followed by a colon and the port. Input strings are split on every colon; only the first two pieces become host and port. This is not a general bracketed IPv6 endpoint parser.
3. Object input casts each item to JSONObject and constructs an address from its host and safeReadInt result. Missing or non-integer port values become zero; a Long outside signed 32-bit range throws.
4. Each conversion constructs a fresh result list/array. Null containers, null elements, wrong element types, invalid ports and malformed strings fail at the operation that needs them.

## How the translation is approached

The mapped C++ code reuses InetSocketAddress, ordered JSON and Java integer parsing. It preserves trailing-empty split behavior and checked narrowing instead of treating every JSON number as a port. Shared address/list types model nullable references; generic native exception classes still differ from Java.

## What the tests tell us

[Read the differential experiment](../../../../../../../../../tools/peer_messages_differential.py). PeerMessagesOracle.java and peer_messages.cpp compare numeric IPv4 forms, IPv6 scopes, unresolved-name equality/hashes, JSON conversion and safeReadInt. Inputs include missing ports, floating values, overflowing integers, extra colons and malformed list elements. Exceptions are collapsed to ERROR; DNS cache and custom JVM networking properties are excluded. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [JSONArray](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [InetSocketAddressIO](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java#L12), [addressesListToJSON](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java#L15), [addressesListToJSONArray](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java#L28), [addressesJSONToList](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java#L38), [addressesJSONArrayToList](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java#L49), [safeReadInt](../../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/messages/InetSocketAddressIO.java#L61).

## Vault/signing follow-up

Vault phrase comparison now shares the Unicode case-folding helper extracted from the native InetSocketAddress support adapter. The unresolved-address equality key keeps its original UTF-16 length and folded code points. Peer-message and peer-state regressions check that this reuse preserves address behavior. See the [experiment and results](../../../../../../../../new-tests/vault-and-signing.md).

## Check your understanding

Predict the port for an object containing "port":"9001". Then explain why the string "127.1:9001:ignored" follows a different path.

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `fbf80a246b7750b29ceb3de8e4615ddc4ccaf84dce7317e86ddcc8b796e530e9`. Mapping: [source-map.json](../../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../../REVIEW.md).

### Node-count integration follow-up

Actual ping greetings now exercise the string-array reader with null, non-string and missing-colon entries. The native method reports the Java null-pointer, class-cast and array-index exception categories before parsing an address. The source still splits on colons and discards trailing empty fields; this change does not add an IPv6-aware parser.
