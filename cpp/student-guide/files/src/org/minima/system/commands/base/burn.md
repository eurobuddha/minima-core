# burn — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `unported`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/burn.java) · `src/org/minima/system/commands/base/burn.cpp` (absent)

## Purpose

burn summarizes transaction burn amounts over the latest one, ten and fifty blocks. The median implementation chooses the lower middle value for an even-sized collection.

## Follow the command

1. For default action list, walk at most fifty parent blocks. Feed each block TxPoW and each found referenced TxPoW into checkBurn; duplicate IDs are not deduplicated.
2. Only objects with isTransaction true contribute. Read their burn amount, including zero. Update the one-block bucket for counter zero, the ten-block bucket below ten, and the fifty-block bucket every time.
3. Each bucket tracks count, total, minimum, maximum and a list. Initial minimum is one billion and maximum is zero; an empty bucket has its minimum changed to zero before output.
4. For the median, sort values descending in place and choose index size/2. For two sorted values 9 and 3 this returns 3, not their average. Compute average as total/count when nonempty.
5. Return txns, max, med, avg and min for each window. Unknown action leaves an empty response. Accumulators are instance fields and are not reset by runCommand; the usual fresh getFunction instance avoids repeated-call accumulation.

## Translation and dependencies

The mapped native counterpart is absent. Reuse the existing Command parameter/reply helpers and the named service behavior when porting; this lesson does not claim that registering a command name supplies its implementation.  Reuse MiniNumber arithmetic and TxPoW burn calculation. Preserve lower-middle selection, no deduplication and instance lifetime; a standard statistical median helper would differ.

## Understanding the tests

No direct native assertion for this absent command is claimed. Proposed fixtures distinguish block offsets 0, 9, 10, 49 and 50; include zero burns, missing references, duplicate IDs and block-as-transaction. Test two/four values, empty windows, unusual negative or above-billion amounts and two calls on one instance. Assert state accumulation explicitly.

- [Shared parameter parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)

No build or test run was performed while authoring this lesson. Proposed tests are future work, not pass results.

## Limits

The command does not validate transactions before counting them. Its accepted action access and runner parameter allow-list are separate concerns, and a successful output is not a chain-wide fee statistic.

## Check your understanding

Why does sorting [2, 8, 4, 6] descending and selecting index 2 produce a median of 4?

Pinned Java SHA-256: `2d550e4d98c4c7367ce73871b69c7f0d3e174b031f311e4a30142c6f7cfe3782`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).
