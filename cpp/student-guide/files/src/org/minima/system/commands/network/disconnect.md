# disconnect — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The counterpart compiles and is registered; full node behavior remains unfinished.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/network/disconnect.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/disconnect.java) | [Native C++ counterpart](../../../../../../../../src/org/minima/system/commands/network/disconnect.cpp) |

## What this file does

Requests disconnection of one peer by UID or all peers.

## Follow the logic

1. Read uid directly from the parameter object; null/missing UID throws No uid specified and a wrong type fails the String cast.
2. For exact lowercase all, post NIO_DISCONNECTALL to the NIO manager.
3. For every other UID, call the manager’s disconnect(uid) method directly. The command does not normalize case or validate UID syntax.
4. After that call returns, set status true and describe the attempted disconnection. The response does not demonstrate that a remote peer has already closed.

## How the translation is approached

The native body reuses Command replies, actual Main/NIO ownership, Message and NIOManager.disconnect. It reads the raw JSON String directly, preserving empty text and whitespace. Exact lowercase all posts the broadcast-disconnect message; every other string is passed unchanged to disconnect. Explicit type checks preserve Java cast errors.

## What the tests tell us

The [peer-command lab](../../../../../../../new-tests/network-peer-commands.md) was written before these tests. Its [222-program corpus](../../../../../../../../tools/network_peer_commands_corpus.py) runs 794 operations against Java and C++, observing replies, metadata, typed failures, queue entries, packet bytes, traffic and real P2P state. Both normal and ASAN/UBSAN comparisons pass.

The fixture reuses PeerRuntimeFixture and P2PManagerFixture through a real Main and NetworkManager. Clients are constructed only after publishing the network owner, because their constructors read it. It then applies deliberately missing owners or incompatible peer-manager inputs. No worker or selector is started. Packet queues and disconnect queues use actual component methods; source/native fixture helpers only set up owners and inspect state afterward.

Independent assertions require valid fixture setup and successful detailed status for healthy owners. For a message, Python builds the expected byte sequence: byte 5, a four-byte UTF-8 length, then the payload bytes. It checks connected valid clients for broadcast, a matching connected UID for targeted sending, and no packets for pending or absent recipients. Queue assertions preserve disconnect message type, raw UID and order. Cases include blank/typed/null parameters, Unicode, embedded zero bytes, an 8,193-character payload, repeated requests, missing owners and runner validation/permissions.

The first real missing-NIO case found generic RuntimeException where Java throws NullPointerException. The shared NIOManager and P2PFunctions owner lookups now preserve that exception class. Runtime-specific null/cast diagnostic prose is normalized; packet bytes and reply values are compared directly.

## Dependencies and reading map

Direct Minima imports: [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [NIOManager](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOManager.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java).

Method navigation (declarations with bodies found in this file): [disconnect](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/disconnect.java#L14), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/disconnect.java#L19), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/disconnect.java#L38), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/disconnect.java#L43), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/disconnect.java#L67).

## Check your understanding

Why is disconnect uid:ALL a different branch from uid:all? What would a callback-recording test need to observe?

## Limits and traceability

These tests stop at real in-memory queues and status generation. They do not prove live peer delivery, asynchronous disconnection, full Main startup, durable storage, exhaustive concurrent owner replacement or arbitrary retained-reference behavior. Existing component tests retain their separate evidence; this counterpart stays partial until full API/parity review is complete.

Pinned Java SHA-256: `2aaae6652023fb052080ca14bc0d63d60985331b49612879f0385dd08f08fdaa`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
