# network — implementation lesson

**Teaching status:** Authored algorithm and test explanation; full API/parity review remains open.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/commands/network/network.java](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/network.java) | [src/org/minima/system/commands/network/network.cpp](../../../../../../../../src/org/minima/system/commands/network/network.cpp) |

## What this file does

Lists direct connections and exposes traffic reset, host selection, restart requests and shared message tracing.

## Follow the logic

1. Read action through Command, defaulting only an absent value to list. Supplied values are strings, trimmed and nonblank; branch names are case-sensitive.
2. For list, read uid with an empty default and obtain NIOClientInfo entries from the connecting and connected maps. Empty uid includes all entries; a nonempty uid stops after its first exact match. This also determines which map wins when the same UID appears in both.
3. Build response.connections, then look up Main’s NetworkManager again and include getStatus(true) if that second network owner is present. The body reads uid although getValidParams advertises only action, so the text runner rejects a supplied uid.
4. For reset, call the actual NIOTraffic.reset, clearing totals and both breakdown maps and recording a new start time. addReadBytes and addWriteBytes do not themselves change the total counters in the pinned source.
5. For recalculateip, call the existing host-selection routine and return the resulting global host. A configured host bypasses enumeration. restart only queues MAIN_NETRESTART and returns Restarting..; it does not wait for the worker.
6. For loggingon/off, call NIO then P2P setFullLogging with an empty filter. MessageProcessor stores tracing in static fields, so the first call already changes shared tracing. A missing second receiver can throw while that change remains visible.
7. Throw CommandException with Invalid action for any other action. Hidden restart and logging branches remain executable even though the short help lists fewer actions.

## How the translation is approached

The native wrapper reuses Command, Main, NetworkManager, NIOManager, NIOClientInfo, NIOTraffic and MessageProcessor. It preserves the repeated owner lookups and the source parameter discrepancy. MainNetworkFixture now binds its NetworkManager UserDB callback to the actual test MinimaDB, matching Main’s normal binding. Optional status-fixture setup supplies fixed initial times and independently seeded totals/breakdowns; it does not replace the manager’s status or reset algorithms.

## What the tests tell us

The [network corpus](../../../../../../../../tools/network_status_commands_corpus.py) supplies 124 programs and 562 operations through the [existing Main driver](../../../../../../../../tools/main_runtime_differential.py). Tests include every action, typed failures, missing owners, direct UID filtering versus runner rejection, configured and real local host discovery, empty lists and duplicate UIDs in separate maps. Explicit assertions check first-match connection state, exact restart queue content, trace changes surviving a failed second receiver, and reset totals/maps/start-time bounds. The [lab](../../../../../../../new-tests/network-status-command.md) records final results. Full metadata and replies also compare with the pinned Java implementation.

## Dependencies and reading map

Direct Minima imports: [Main](../../../../../../../../../upstream-study/src/org/minima/system/Main.java), [Command](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java), [CommandException](../../../../../../../../../upstream-study/src/org/minima/system/commands/CommandException.java), [NetworkManager](../../../../../../../../../upstream-study/src/org/minima/system/network/NetworkManager.java), [NIOClientInfo](../../../../../../../../../upstream-study/src/org/minima/system/network/minima/NIOClientInfo.java), [GeneralParams](../../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java), [JSONArray](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONArray.java), [JSONObject](../../../../../../../../../upstream-study/src/org/minima/utils/json/JSONObject.java).

Method navigation (declarations with bodies found in this file): [network](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/network.java#L17), [getFullHelp](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/network.java#L22), [getValidParams](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/network.java#L42), [runCommand](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/network.java#L47), [getFunction](../../../../../../../../../upstream-study/src/org/minima/system/commands/network/network.java#L128).

## Check your understanding

If the same UID appears in both the connecting and connected maps, which entry does direct list filtering return? Why can loggingon fail while tracing is already enabled for every MessageProcessor?

## Limits and traceability

Workers remain unstarted. Restart is a queue assertion, not a completed restart. Initial fixture clocks and UTC rendering are fixed; live reset times are bounded before normalization, and reset date prose is excluded. Enhanced null/cast diagnostic expressions are normalized by the existing driver. Concurrent owner replacement, all enabled P2P status failures, real RPC startup and full-node persistence/startup remain unverified. The counterpart stays implemented_partial and parity not_verified.

Pinned Java SHA-256: `4ee7d2840c232f79572cd6d69c38d342f772576b61c15388d10f23009fd2b10e`. Mapping: [source-map.json](../../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../../REVIEW.md).
