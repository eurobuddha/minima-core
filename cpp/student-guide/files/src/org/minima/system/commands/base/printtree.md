# printtree — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/printtree.java) · [Native counterpart](../../../../../../../../src/org/minima/system/commands/base/printtree.cpp)

## Purpose

printtree assembles human-readable chain and optionally cascade renderings, preserving their literal whitespace.

## Follow the command

1. Read depth as a trimmed string with default 32 and parse it using Integer.parseInt, rather than MiniNumber. Fractional depth therefore fails integer parsing.
2. Read cascade with default false and compare exactly with true.
3. If enabled, call Cascade.printCascade first and prefix its result with a newline under cascade.
4. Call TxPowTree.printTree(depth), prefix a newline under chain, then return the response. Negative depths are passed through to the tree renderer without command-level validation.

## Translation and dependencies

The native counterpart reuses the actual MinimaDB singleton, existing tree/cascade renderers and Java integer parser. It retains cascade-before-chain order, exact flags, leading newlines and parameter validation. Source review also corrected the renderer’s signed-32-bit depth subtraction.

## Understanding the tests

The [chain command lab](../../../../../../../new-tests/chain-commands.md) explains the actual Java/native fixture, fail-fast missing-registry baseline, populated and empty chains, forks, MMR entries, direct/runner errors and current-singleton selection. Its 78-program UTC corpus passes in both builds. The expanded four-zone corpus passes **324 programs and 4,096 operations** in both builds, including fractional/extreme block and header timestamps. Follow the lab for regression evidence and limitations.

The fixture compares exact replies, exception categories/messages subject to the stated null/class-cast normalization, logger bodies, database identities, locks, file bytes and SQL/Main effects. It reuses real child implementations with recording external receivers. It does not claim native H2 durability or full startup.


## Limits

String rendering tests should compare literal layout, while topology correctness belongs to tree/cascade tests. Populated-tree extreme integer depths and arbitrary concurrent mutation remain unverified.

## Check your understanding

Why does depth:1.5 fail here even though many other commands accept MiniNumber parameters?

Pinned Java SHA-256: `064007157379da49d14f99e1cce550b39ef230de9ade80be6eb0d2927c1bea78`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
