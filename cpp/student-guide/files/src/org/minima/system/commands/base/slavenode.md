# slavenode — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/slavenode.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/base/slavenode.cpp)

## Purpose

slavenode updates and reports the persisted slave-node preference. Runtime behavior changes require restart; this command does not connect to the master immediately.

## Follow the command

1. Obtain UserDB and create a response.
2. If enable exists, add RESTART REQUIRED, parse the boolean, and either require a host and call setSlaveNode(true,host), or call setSlaveNode(false,empty). The disable branch does not parse a supplied host.
3. Call MinimaDB.saveUserDB even when enable was absent and the invocation only reads settings.
4. Read isSlaveNode/getSlaveNodeHost after saving, populate enabled/master, and return. Saving or getter failure can happen after preferences changed.

## Translation and dependencies

The native command now uses the actual MinimaDB owner and existing Command parameter/reply helpers, and is registered for dispatch. It reuses UserDB accessors and MinimaDB persistence. Do not substitute an immediate network connection or make the query-only path skip its source save call.

## Understanding the tests

The tests compare command replies, in-memory enabled/host values and exact temporary preference bytes for query, enable, disable and malformed parameters. They disable saving, change memory, reload the earlier file, then re-enable saving. Owner recreation followed by explicit reload checks recovery of saved values. Setter/save/getter order is read from the source; the fixture does not intercept each of those calls or inject filesystem failures.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

Both native builds and all six selected regression suites pass. The expanded address group passes 134 programs with 276 operations and 3,693 SQL calls in each build. See the [address-command lab](../../../../../../../new-tests/address-commands.md) for inputs, operation order and limitations.

## Limits

The fixture now uses actual temporary preference files and compares exact bytes, disabled-save behavior and reloads after owner recreation. Full node restart, filesystem failures and crash-safe persistence remain unverified.

## Check your understanding

Why does invoking slavenode with no options still perform a write-related service call?

Pinned Java SHA-256: `6e3f0b6fb2ef8ce7bb861a08cac72585f32980fc7c473633f50b097217bbca05`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
