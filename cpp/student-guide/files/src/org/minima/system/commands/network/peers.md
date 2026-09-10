# peers — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The native command compiles and finite Java/C++ comparisons pass; full inherited API and runtime parity remain open.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/network/peers.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/peers.java) | [src/org/minima/system/commands/network/peers.cpp](../../../../../../../../src/org/minima/system/commands/network/peers.cpp) |

## What this file does

Lists, checks, imports or publishes the P2P peer-address list.

## Follow the logic

1. If P2P is disabled or the node is a slave, return immediately with peers-list empty, havepeers true and a disabled message. This key contains a hyphen; ordinary list output uses peerslist.
2. list asks getPeersList for a randomized bounded list, counts comma-separated entries and corrects an empty string to size zero. forcecheck posts PEERS_FORCEFULLCHECK, then reports the list without that empty correction, so its empty size can differ.
3. addpeers reads a required string; a prefix beginning http triggers an HTTP GET. Tokenize by comma, discarding empty tokens. Parse each endpoint through connect.createConnectMessage; accepted endpoints become InetSocketAddress values and PEERS_CHECKPEERS messages with force=true.
4. publish writes up to the helper’s nominal 20 peers to a base-path file using the platform default byte encoding. fetch rejects an existing local file, otherwise downloads the url, tokenizes and posts checks similarly.
5. getPeersList obtains a copy, shuffles it, then stops only when counter > max. Consequently a nonnegative maximum N can return N+1 peers. It formats resolved IP addresses and strips the final comma on the successful path.
6. The helper catches exceptions and returns accumulated text. If failure precedes comma cleanup, partial output may retain a trailing comma. Importing peers is incremental: a later invalid socket port can fail after earlier check messages were posted.

## How the translation is approached

The native command reuses connect’s endpoint parser, InetSocketAddress, Main/P2P/checker state, MiniFile, RPCClient and the existing peer shuffle. Parsing an integer port and constructing a valid socket address remain separate steps. Random selections are checked independently for membership and cardinality, then normalized for comparison.

## What the tests tell us

The [peer-command lab](../../../../../../../new-tests/peers-command.md) explains 271 programs / 1,355 operations passing in normal and sanitizer builds. Checks use real loopback HTTP (48 requests per runtime), MiniFile writes and unstarted checker queues. They cover disabled/slave replies, inclusive limits, empty counts, malformed owners, unresolved addresses, duplicates, partial posting, downloads and file failures. Actual queue fields and file bytes are compared.

## Dependencies and reading map

Direct Minima imports: [MinimaDB](../../../../../../../../../upstream-study/src/org/minima/database/MinimaDB.java), [MiniNumber](../../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java), [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [P2PManager](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PManager.java), [P2PPeersChecker](../../../../../../../../../upstream-study/src/org/minima/system/network/p2p/P2PPeersChecker.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [MiniFile](../../../../../../../../../upstream-study/src/org/minima/utils/MiniFile.java), [MinimaLogger](../../../../../../../../../upstream-study/src/org/minima/utils/MinimaLogger.java), [RPCClient](../../../../../../../../../upstream-study/src/org/minima/utils/RPCClient.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java), [Message](../../../../../../../../../upstream-study/src/org/minima/utils/messages/Message.java).

Method navigation (declarations with bodies found in this file): [peers](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/peers.java#L26), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/peers.java#L31), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/peers.java#L55), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/peers.java#L60), [getPeersList](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/peers.java#L245), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/peers.java#L292).

## Check your understanding

Why can max:0 still publish one peer? Trace the counter comparison before incrementing it.

## Limits and traceability

File operations, loopback downloading, random selection and queue submission are implemented and tested. Live peer admission, public DNS, concurrent scheduling, arbitrary subclass/reference behavior and complete native-node integration remain unverified.

Pinned Java SHA-256: `a106f44f73f181d0fe4d7255eef7bdb80dc55a5bb715dcdfd1b36b173d0d27df`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
