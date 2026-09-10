# printmmr — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/printmmr.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/base/printmmr.cpp)

## Purpose

printmmr returns the tip node’s MMR JSON. It does not reconstruct an MMR or print each entry itself.

## Follow the command

1. Create the common reply.
2. Follow MinimaDB→TxPoWTree→tip→MMR with no intermediate null checks.
3. Call that MMR’s toJSON, attach it as response, and return. getFunction supplies a fresh command instance.

## Translation and dependencies

The native counterpart uses the actual MinimaDB singleton, tree tip and existing MMR JSON implementation. Checked native pointer dereferences preserve the Java null-pointer category without undefined behavior. It does not introduce an alternate MMR serializer.

## Understanding the tests

The [chain command lab](../../../../../../../new-tests/chain-commands.md) explains the actual Java/native fixture, fail-fast missing-registry baseline, populated and empty chains, forks, MMR entries, direct/runner errors and current-singleton selection. Its 78-program UTC corpus passes in both builds. The expanded four-zone corpus passes **324 programs and 4,096 operations** in both builds, including fractional/extreme block and header timestamps. Follow the lab for regression evidence and limitations.

The fixture compares exact replies, exception categories/messages subject to the stated null/class-cast normalization, logger bodies, database identities, locks, file bytes and SQL/Main effects. It reuses real child implementations with recording external receivers. It does not claim native H2 durability or full startup.


## Limits

The help text’s description of row numbering is not implemented by this wrapper. Establish row meaning from MMR source/tests, not the command’s prose.

## Check your understanding

Which object decides the entries and field names in the response?

Pinned Java SHA-256: `d6fa03ce729aa8175006c6ea9c2604e69a67b45be593244fc2e752ba2b2ed3e6`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
