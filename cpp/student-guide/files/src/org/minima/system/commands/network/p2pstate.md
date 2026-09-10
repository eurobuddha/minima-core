# p2pstate — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The counterpart compiles and is registered; full node behavior remains unfinished.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/network/p2pstate.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/p2pstate.java) | [Native C++ counterpart](../../../../../../../../src/org/minima/system/commands/network/p2pstate.cpp) |

## What this file does

Returns the P2P manager’s detailed status as a top-level reply field.

## Follow the logic

1. Create the standard command reply.
2. Resolve Main, then NetworkManager, then its P2P manager, and cast that receiver to P2PManager.
3. Call getStatus(true) and store the result under the top-level key p2p-state. It does not use the usual response key.
4. Return the reply. Null receivers or an incompatible manager fail before a status object can be returned.

## How the translation is approached

The native body resolves the real Main-owned NetworkManager and uses dynamic_pointer_cast to P2PManager. A nonnull wrong type raises ClassCastException; a null peer remains null until its method is required. The actual getStatus(true) supplies the reply, under p2p-state rather than response.

## What the tests tell us

The [peer-command lab](../../../../../../../new-tests/network-peer-commands.md) was written before these tests. Its [222-program corpus](../../../../../../../../tools/network_peer_commands_corpus.py) runs 794 operations against Java and C++, observing replies, metadata, typed failures, queue entries, packet bytes, traffic and real P2P state. Both normal and ASAN/UBSAN comparisons pass.

The fixture reuses PeerRuntimeFixture and P2PManagerFixture through a real Main and NetworkManager. Clients are constructed only after publishing the network owner, because their constructors read it. It then applies deliberately missing owners or incompatible peer-manager inputs. No worker or selector is started. Packet queues and disconnect queues use actual component methods; source/native fixture helpers only set up owners and inspect state afterward.

Independent assertions require valid fixture setup and successful detailed status for healthy owners. For a message, Python builds the expected byte sequence: byte 5, a four-byte UTF-8 length, then the payload bytes. It checks connected valid clients for broadcast, a matching connected UID for targeted sending, and no packets for pending or absent recipients. Queue assertions preserve disconnect message type, raw UID and order. Cases include blank/typed/null parameters, Unicode, embedded zero bytes, an 8,193-character payload, repeated requests, missing owners and runner validation/permissions.

The first real missing-NIO case found generic RuntimeException where Java throws NullPointerException. The shared NIOManager and P2PFunctions owner lookups now preserve that exception class. Runtime-specific null/cast diagnostic prose is normalized; packet bytes and reply values are compared directly.

## Dependencies and reading map

Direct Minima imports: [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [P2PManager](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [p2pstate](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/p2pstate.java#L10), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/p2pstate.java#L15), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/p2pstate.java#L28), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/p2pstate.java#L38).

## Check your understanding

What would break in a client that assumes every successful command stores data under response?

## Limits and traceability

These tests stop at real in-memory queues and status generation. They do not prove live peer delivery, asynchronous disconnection, full Main startup, durable storage, exhaustive concurrent owner replacement or arbitrary retained-reference behavior. Existing component tests retain their separate evidence; this counterpart stays partial until full API/parity review is complete.

Pinned Java SHA-256: `10404509966c65e56502d6b33eab69d2690fcf2f96739e661b17d85c6a3fe82c`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
