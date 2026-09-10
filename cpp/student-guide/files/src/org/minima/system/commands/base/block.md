# block — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/block.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/base/block.cpp)

## Purpose

block reports the currently selected chain tip’s number, ID, timestamp and human-readable date.

## Follow the command

1. Create the reply, obtain MinimaDB’s TxPoWTree, then read its tip.
2. If the tip is null, throw CommandException with NO Blocks yet.. before constructing a response.
3. Read the tip’s TxPoW and emit block and timemilli as decimal strings, hash as its TxPoW ID, and date using Date(timestamp.getAsLong()).toString().
4. Attach the response and return. The command does not search archives or choose a tip itself.

## Translation and dependencies

The native counterpart uses the actual MinimaDB singleton and tree tip, existing TxPoW getters and Java-compatible date formatting. It preserves the source JSON string fields, explicit no-block CommandException, metadata and fresh factory.

## Understanding the tests

The [chain command lab](../../../../../../../new-tests/chain-commands.md) explains the actual Java/native fixture, fail-fast missing-registry baseline, populated and empty chains, forks, MMR entries, direct/runner errors and current-singleton selection. Its 78-program UTC corpus passes in both builds. The expanded four-zone corpus passes **324 programs and 4,096 operations** in both builds, including fractional/extreme block and header timestamps. Follow the lab for regression evidence and limitations.

The fixture compares exact replies, exception categories/messages subject to the stated null/class-cast normalization, logger bodies, database identities, locks, file bytes and SQL/Main effects. It reuses real child implementations with recording external receivers. It does not claim native H2 durability or full startup.


## Limits

A stub tip tests the command’s projection only; choosing the correct chain tip requires the tree/consensus tests. The date is runtime-zone dependent.

## Check your understanding

Why would emitting a JSON number for block differ from the source even when its decimal digits are identical?

Pinned Java SHA-256: `4105fe999010225fd7a01eef764721269b4593155d465c9e5f8dd74fb939011b`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
