# P2PFunctions — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/network/p2p/P2PFunctions.java](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java) | [src/org/minima/system/network/p2p/P2PFunctions.cpp](../../../../../../../../src/org/minima/system/network/p2p/P2PFunctions.cpp) |

## Peer-command integration update

The [peer-command lab](../../../../../../../new-tests/network-peer-commands.md) adds actual Main-owned command execution and typed missing-NIO checks. The owner lookup now throws JavaNullPointerException for an absent binding or NIO owner instead of a generic runtime failure. The 222-program comparison passes in both builds; full live startup and delivery remain separate work.

## Interface regression follow-up

The real-interface comparison found a unique-local IPv6 hostname losing its interface suffix. P2PEnvironment now uses the interface index for its numeric fallback, following the inspected OpenJDK enumeration. Existing reverse-name validation and cached-set ownership remain in place. All 672 P2P-function programs, 3,670 operations and 39 localhost HTTP cases pass in both builds after this correction. Available interface addresses still depend on the machine.

## What this file does

Bridge discovery policy to NIO connection messages, cached interface names, peer exclusions, logging and availability checks.

## Follow the logic

1. Cache and return the live local-hostname set. Only interface SocketException triggers the fallback names localhost, 127.0.0.1 and 127.0.1.1. The invalid-peer set is separate and mutable; adding an existing entry avoids another log.
2. checkConnect rejects an explicitly invalid host:port first, then strings with more than one colon. Its local-address rule is a literal prefix list, including broad prefixes such as 172. and 198.; it is not a CIDR parser.
3. Unless ALLOW_ALL_IP is enabled, a local prefix rejects connection. A caught error during this local check is logged and processing continues.
4. Reject a matching host/port only when its client is still connecting. Already connected entries do not trigger that particular guard. A permitted request posts NIO_CONNECT and returns true; direct connect bypasses these policy checks.
5. Connection queries build NIOClientInfo lists; sends serialize JSON as MiniString in MSG_P2P and an empty UID broadcasts. Logging levels have explicit equality rules, not a simple numeric threshold.
6. isNetAvailable opens the configured source URL, connects, opens input and closes it. Ordinary exceptions yield false; this probes that operation, not every peer route.

## How the translation is approached

The translation reuses NIOManager, message framing, JavaStringSet and native interface/HTTP adapters. Required runtime lookup and optional platform/log receivers keep source policy separate from external effects. The sibling blocking network design was not substituted.

## What the tests tell us

[Read the differential experiment](../../../../../../../../tools/p2p_functions_differential.py). The actual helper methods run on controlled managers with real native maps/queues and codecs. Programs compare invalid/local/IPv6 ordering, pending versus connected clients, send packets, log bodies and live cache aliases. A separate loopback HTTP experiment checks response/redirect paths; the default public HTTPS provider and full Main bindings remain outside that evidence. No test was rerun while authoring this lesson; this describes the experiment, not a new passing result.

## Dependencies and reading map

Direct Minima imports: [MiniString](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [NIOClientInfo](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java), [NIOManager](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java), [NIOMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOMessage.java), [P2PParams](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/params/P2PParams.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java).

Method navigation (declarations with bodies found in this file): [getLocalAddresses](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L61), [addInvalidPeer](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L81), [isInvalidPeer](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L90), [clearInvalidPeers](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L95), [isIPv6](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L99), [isIPLocal](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L103), [isNetAvailable](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L115), [connect](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L131), [checkConnect](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L141), [disconnect](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L194), [getAllConnections](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L203), [getAllConnectedConnections](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L207), [getNIOCLientInfo](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L221), [sendP2PMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L238), [sendP2PMessageAll](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L251), [log_node_runner](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L254), [log_info](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L261), [log_debug](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L267), [getAllNetworkInterfaceAddresses](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L273), [main](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PFunctions.java#L285).

## Check your understanding

Why can a hostname beginning localhost.evil be rejected by this exact source? Explain why replacing the rule with a standards-based IP parser would change a 1:1 port.

## Limits and traceability

This lesson explains the inspected source and finite test scope. It does not certify full API parity, live network integration or a working native node. The mapping remains authoritative.

Pinned Java SHA-256: `7dd3030e44b51fb8f2eb497ea58edabfeaf7c2601e960fafd91f04b996087513`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).

## Follow-up: complete source entry point

The source main prints NET:true or NET:false after the real isNetAvailable call. Existing offline URL fixtures exercise opening, connecting, obtaining the stream and closing it. Ordinary failures return false; Java Error equivalents now escape before output. Tests cover IO, AssertionError and OutOfMemoryError at every stage.

See the [entry-point lab](../../../../../../../new-tests/core-main-methods.md) for logic, tests and limits.
This addition does not establish whole-file parity.
