# healthcheck — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/healthcheck.java) · `src/org/minima/system/commands/base/healthcheck.cpp` (absent)

## Purpose

healthcheck reports tip/root heights and checks whether the cascade tip is immediately before the tree root. The active source does not produce the Maxima fields advertised in help.

## Follow the command

1. Get the chain tip and reject null with No TIP block found.. ?.
2. Get the root, emit tip/root heights as strings, and compute chainlength as tip−root, without adding one.
3. If the cascade has a tip, read its height, set tipcorrect to equality with tree root−1, and include cascade length.
4. If no cascade tip exists, emit the literal string nocacade under cascade. Return the response; there are no proof, parent-hash or Maxima checks in this body.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse tree/cascade accessors and MiniNumber arithmetic while preserving response types and the literal source spelling.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed tests use a single-node chain (length zero), adjacent/nonadjacent cascade heights, no cascade, missing tip/root and changed values between repeated lookups. Exact output types matter because cascade switches between object and string.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

The height adjacency boolean is a limited consistency check, not full chain/cascade cryptographic validation.

## Check your understanding

Why does a chain containing just its root report chainlength zero?

Pinned Java SHA-256: `cff60ae34f39db98ac66f3222925f953da734bf85e43dc87234227d194d2df0b`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
