# burn — teaching guide

**Teaching status:** Authored source explanation, translation status, and test reasoning. This is not a parity certification.

**Port status:** `implemented_partial`; parity `not_verified`.

[Pinned Java source](../../../../../../../../../upstream-study/src/org/minima/system/commands/base/burn.java) · [C++ counterpart](../../../../../../../../src/org/minima/system/commands/base/burn.cpp)

## Purpose

burn summarizes transaction burn amounts over the latest one, ten and fifty blocks. The median implementation chooses the lower middle value for an even-sized collection.

## Follow the command

1. For default action list, walk at most fifty parent blocks. Feed each block TxPoW and each found referenced TxPoW into checkBurn; duplicate IDs are not deduplicated.
2. Only objects with isTransaction true contribute. Read their burn amount, including zero. Update the one-block bucket for counter zero, the ten-block bucket below ten, and the fifty-block bucket every time.
3. Each bucket tracks count, total, minimum, maximum and a list. Initial minimum is one billion and maximum is zero; an empty bucket has its minimum changed to zero before output.
4. For the median, sort values descending in place and choose index size/2. For two sorted values 9 and 3 this returns 3, not their average. Compute average as total/count when nonempty.
5. Return txns, max, med, avg and min for each window. Unknown action leaves an empty response. Accumulators are instance fields and are not reset by runCommand; the usual fresh getFunction instance avoids repeated-call accumulation.

## Translation and dependencies

The native command reuses Command parameter/reply helpers, MiniNumber arithmetic,
TxPoW.getBurn, tree nodes and TxPoWDB RAM/SQL lookup. It retains three accumulator
windows and exposes overridable checkBurn/getMediaValue methods. Each burn amount
is shared between its window lists; median returns the selected shared element.

The [nullable-number sort](../../../../../../../../src/support/JavaNullableNumberSort.hpp)
adapts the installed OpenJDK TimSort run and insertion sequence. A comparator
exception can leave an externally visible partially sorted list; ordinary C++
stable_sort did not preserve that state. Completed non-null runs use stable merge.
This specialized helper does not implement arbitrary comparator side effects.

## Understanding the tests

The [burn-metrics lab](../../../../../../../new-tests/burn-metrics.md) was written
before testing. Java generates serialized TxPoWs with known input/output amounts;
the same data loads into both real tree/database implementations. Compare exact
replies and recording-SQL effects. Independently calculate windows using decimal
arithmetic, including the 44-decimal-place average truncation.

Tests distinguish offsets 0, 9, 10, 49 and 50, missing references, duplicate IDs,
zero/negative/above-billion amounts and blocks that are also transactions. Calls
on one instance accumulate; a factory-created object resets. Running once on an
empty tree lowers minimum sentinels to zero, affecting later positive burns.
Public helper counters include negative values and integers above fifty.

Sort cases check descending order, lower-middle selection, stable ties, returned
object identity, null collections and list state after a null-element exception.
Lists around the 32-element merge threshold expose more than a normal median test.
The direct action parameter and runner allow-list are tested separately.

- [Shared parsing and reply envelope](../../../../../../../../../upstream-study/src/org/minima/system/commands/Command.java)
- [Comparison driver](../../../../../../../../tools/burn_metrics_differential.py)
- [Validation evidence](../../../../../../../../validation/burn-metrics-work-in-progress)

Both final builds pass 364 programs / 4,361 operations. Repeated empty-list
results retain identity. The lab records the coverage and remaining limits.

## Limits

The command does not validate transactions or filter token IDs before counting them. Its accepted action access and runner parameter allow-list are separate concerns, and a successful output is not a chain-wide fee statistic.

## Check your understanding

Why does sorting [2, 8, 4, 6] descending and selecting index 2 produce a median of 4?

Pinned Java SHA-256: `2d550e4d98c4c7367ce73871b69c7f0d3e174b031f311e4a30142c6f7cfe3782`. [Mapping](../../../../../../../../source-map.json) · [Project limitations](../../../../../../../../REVIEW.md).

## Block-ID iteration follow-up

The block transaction loop now uses an owned checked range while looking up each transaction and collecting burn data. This reuses the shared ID storage without changing the burn calculation. See the [iteration lab](../../../../../../../new-tests/txbody-id-iteration.md) for the source rules, actual consumer test, current validation and remaining limits.
