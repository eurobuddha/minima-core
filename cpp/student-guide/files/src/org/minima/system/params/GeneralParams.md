# GeneralParams — Mutable process settings and the meaning of reset

**Teaching status:** Authored source algorithm, native adaptation and test explanation; complete parity remains unverified.

**Port status:** `implemented_partial`; parity `not_verified`. The mapped C++ file exists.

| Source | Destination |
|---|---|
| [src/org/minima/system/params/GeneralParams.java](../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java) | [src/org/minima/system/params/GeneralParams.cpp](../../../../../../../src/org/minima/system/params/GeneralParams.cpp) |

## What this file does

GeneralParams is a collection of process-wide settings. It has no per-node object constructor and performs no networking, mining or database work itself. Other components read its flags and values. Its one explicit method, `resetDefaults`, assigns a selected set of fields again because an embedding process can survive a node shutdown.

The fields group naturally into device/link policy, startup modes, file locations and ports, MDS/RPC authentication, peer discovery, relay and retention limits, logging, MegaMMR pruning, SQL/JNI options and seed configuration. Read the declarations for each exact value. A label such as `CLEAN` expresses a request consumed elsewhere; assigning it here does not delete a directory.

## Initialization and reset are different programs

The C++ header declares static fields and the `.cpp` defines their initial values. Java performs corresponding static initialization when its class initializes. Both bodies then implement the explicit reset assignments in source order.

| Setting | Initial value | After `resetDefaults` |
|---|---|---|
| `MINIMA_PORT` | 9001 | 9001 |
| MDS file, MDS command, RPC ports | Main port +2, +3, +4 | Recomputed after main port becomes 9001 |
| `MAX_RELAY_OUTPUTCOINS` | 32 | 15 |
| `MAX_SPLIT_COINS` | 20 | 10 |
| `MAX_RELAY_STORESTATESIZE` | 65536 | Not assigned by reset |
| `SHOW_NETWORK_POLLS` | true | Not assigned by reset |
| `DB_IGNORE_LOCKS` | false | Not assigned by reset |

The last three rows retain their current values across reset. Reset also does not restore GlobalParams or TestParams. Replacing the reset body with a generic reload of declaration defaults would change the pinned program. This is a good example of why a port follows executable statements rather than the apparent promise of a method name.

A port field initialized with `MINIMA_PORT+4` is an ordinary stored integer, not a formula that automatically recalculates whenever the main port changes. The `port` parameter consumer only assigns MINIMA_PORT. Any later startup synchronization belongs to its caller, not this declaration.

## Native adaptations and limits

Java int and long fields use native int32_t and int64_t; Minima quantities retain MiniNumber. Strings and numbers are native values. This avoids substituting floating-point approximations for source decimals, but it does not establish every Java null/reference-alias case. The seven logging switches changed by logs now use std::atomic<bool>, following the existing TxPoWSqlDB logging flags. Their loads and stores are individually synchronized so native workers can read them while a command writes them. Other global settings retain their existing storage. This does not make a multi-flag command or reset atomic, and tests that sequence writes and snapshots cannot certify every full-node interleaving.

## Test evidence shared by these settings

[ParamConfigurerOracle.java](../../../../../../../support/ParamConfigurerOracle.java) snapshots the actual Java static fields after each operation. [param_configurer.cpp](../../../../../../../support/param_configurer.cpp) uses [ParamSnapshots.hpp](../../../../../../../support/ParamSnapshots.hpp) to observe the corresponding native fields. Before each scenario, both harnesses restore a captured baseline; a `reset` operation inside a scenario calls the production `GeneralParams.resetDefaults` instead. Those are different operations.

The [comparison driver](../../../../../../../tools/param_configurer_differential.py) tests argument, environment and file inputs, repeated configuration, reset, test-setting overrides, malformed UTF-8 and numbers, map traversal, filesystem effects and loopback HTTP. Existing normal and sanitizer [reports](../../../../../../../build/param-configurer-results.json) each record **719 scenarios, 67 handlers, four HTTP requests per runtime and three subprocess checks**, with zero failures. Those are suite totals shared by this family, not separate totals for each file. The logs integration reran the affected parameter and worker tests after changing flag storage. All 25 selected tests pass in both builds, including these 719 parameter scenarios and the original source test. The [logging-flags lab](../../../../../../new-tests/logging-flags-command.md) links to checkpoint evidence.

The driver supplies Java's observed enum identity hashes to the native map so both traverse the same arbitrary identity ordering. It normalizes logger prefixes and selected exception diagnostics while retaining their categories. The fixtures do not prove all JVM initialization, sharing, exception or concurrency semantics, and do not start a complete native node.

## Dependencies and reading map

Direct Minima imports: [MiniNumber](../../../../../../../../upstream-study/src/org/minima/objects/base/MiniNumber.java).

Method navigation (declarations with bodies found in this file): [resetDefaults](../../../../../../../../upstream-study/src/org/minima/system/params/GeneralParams.java#L396).

## Check your understanding

Set `DB_IGNORE_LOCKS=true`, change `MAX_RELAY_OUTPUTCOINS` to 99, and call reset. Predict both fields. **Answer:** the lock flag remains true and the relay limit becomes 15. Explain this by pointing to an assignment that exists and one that is absent, rather than reasoning from the word “defaults.”

## Traceability

Pinned Java SHA-256: `212cd6a69a8378f9c5ce434103da062461c3fdee41b8efb828f89b1831e195ff`. Mapping: [source-map.json](../../../../../../../source-map.json). Project limitations: [REVIEW.md](../../../../../../../REVIEW.md).
