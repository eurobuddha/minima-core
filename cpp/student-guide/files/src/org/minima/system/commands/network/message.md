# message — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The counterpart compiles and is registered; full node behavior remains unfinished.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/network/message.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/message.java) | [Native C++ counterpart](../../../../../../../../src/org/minima/system/commands/network/message.cpp) |

## What this file does

Wraps user text in MiniString and asks the NIO manager to send a generic peer message.

## Follow the logic

1. Read data as a raw String parameter and reject missing/null data with No data specified.
2. Read optional uid. If null, replace it with the empty string and describe a broadcast; otherwise describe a message to the supplied UID, including an explicitly empty UID.
3. Construct MiniString from the data, then call sendNetworkMessage(uid, MSG_GENMESSAGE, payload). UTF-8 normalization belongs to that wrapper.
4. Return the reply after the send call. The command contains no delivery acknowledgement or retry mechanism.

## How the translation is approached

The native body reuses raw JSON String access, MiniString and NIOManager.sendNetworkMessage. It constructs the source GENMESSAGE payload and performs Main.getNIOManager even though the subsequent NIO method is static: Java evaluates that receiver expression too. A returned null NIO reference does not fail merely because a static call was written through it; the actual send later resolves its required owner.

## What the tests tell us

The [peer-command lab](../../../../../../../new-tests/network-peer-commands.md) was written before these tests. Its [222-program corpus](../../../../../../../../tools/network_peer_commands_corpus.py) runs 794 operations against Java and C++, observing replies, metadata, typed failures, queue entries, packet bytes, traffic and real P2P state. Both normal and ASAN/UBSAN comparisons pass.

The fixture reuses PeerRuntimeFixture and P2PManagerFixture through a real Main and NetworkManager. Clients are constructed only after publishing the network owner, because their constructors read it. It then applies deliberately missing owners or incompatible peer-manager inputs. No worker or selector is started. Packet queues and disconnect queues use actual component methods; source/native fixture helpers only set up owners and inspect state afterward.

Independent assertions require valid fixture setup and successful detailed status for healthy owners. For a message, Python builds the expected byte sequence: byte 5, a four-byte UTF-8 length, then the payload bytes. It checks connected valid clients for broadcast, a matching connected UID for targeted sending, and no packets for pending or absent recipients. Queue assertions preserve disconnect message type, raw UID and order. Cases include blank/typed/null parameters, Unicode, embedded zero bytes, an 8,193-character payload, repeated requests, missing owners and runner validation/permissions.

The first real missing-NIO case found generic RuntimeException where Java throws NullPointerException. The shared NIOManager and P2PFunctions owner lookups now preserve that exception class. Runtime-specific null/cast diagnostic prose is normalized; packet bytes and reply values are compared directly.

## Dependencies and reading map

Direct Minima imports: [MiniString](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniString.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [NIOMessage](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOMessage.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [message](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/message.java#L14), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/message.java#L19), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/message.java#L37), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/message.java#L42), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/message.java#L70).

## Check your understanding

Can omitted uid and explicitly empty uid produce the same send arguments but different reply wording? Trace the branches.

## Limits and traceability

These tests stop at real in-memory queues and status generation. They do not prove live peer delivery, asynchronous disconnection, full Main startup, durable storage, exhaustive concurrent owner replacement or arbitrary retained-reference behavior. Existing component tests retain their separate evidence; this counterpart stays partial until full API/parity review is complete.

Pinned Java SHA-256: `7ef5033ca7edf50a7726a30416aa494c383305815d7b5b33c83a279f4872cce6`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
